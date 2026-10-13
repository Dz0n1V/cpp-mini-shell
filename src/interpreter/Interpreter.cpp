#include "Interpreter.h"
#include "NewlineGuardBuf.h"
#include "../lexer/Lexer.h"
#include "../parser/CommandLineParser.h"
#include "../commands/CommandFactory.h"
#include "../exceptions/CLIException.h"
#include "../exceptions/SemanticException.h"
#include "../exceptions/OSException.h"
#include "../lexer/Token.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

Interpreter::Interpreter() : prompt("$") {}

// ---------------------------------------------------------------------------
// InterpreterContext interface
// ---------------------------------------------------------------------------

void Interpreter::setPrompt(const std::string& newPrompt) {
    prompt = newPrompt;
}

void Interpreter::runBatchFile(const std::string& filename,
                                std::ostream& defaultOut,
                                std::ostream& defaultErr)
{
    std::ifstream ifs(filename);
    if (!ifs) {
        throw OSException("cannot open batch file: " + filename);
    }
    runStream(ifs, defaultOut, defaultErr);
}

// ---------------------------------------------------------------------------
// Line reading (max 512 chars, discard excess)
// ---------------------------------------------------------------------------

std::string Interpreter::readLine(std::istream& input) {
    constexpr int MAX_LINE = 512;
    std::string line;
    line.reserve(MAX_LINE);

    int ch;
    int count = 0;

    while ((ch = input.get()) != EOF && static_cast<char>(ch) != '\n') {
        if (count < MAX_LINE) {
            line += static_cast<char>(ch);
            ++count;
        }
        // Characters beyond 512 are silently discarded.
    }

    return line;
}

// ---------------------------------------------------------------------------
// REPL entry point
// ---------------------------------------------------------------------------

void Interpreter::run() {
    runStream(std::cin, std::cout, std::cerr);
}

void Interpreter::runStream(std::istream& input,
                              std::ostream& defaultOut,
                              std::ostream& defaultErr)
{
    // Display prompt only when the source is the real stdin.
    const bool interactive = (&input == &std::cin);

    // Let formatError() know whether the prompt is visible on screen so it
    // can calculate the correct caret offset.
    const bool prevPromptVisible = promptVisible;
    promptVisible = interactive;

    // Wrap defaultOut in a NewlineGuardBuf so we can guarantee that the
    // prompt always starts on a fresh line, even when a command does not
    // terminate its output with '\n'.
    NewlineGuardBuf guardBuf(defaultOut.rdbuf());
    std::ostream guardOut(&guardBuf);

    // Mirror format flags, locale, etc. from the original stream.
    guardOut.copyfmt(defaultOut);

    // Guarantee the very first prompt starts on a fresh line.
    guardBuf.ensureNewline();

    while (true) {
        if (interactive) {
            guardOut << prompt << ' ';
            guardOut.flush();
        }

        std::string line = readLine(input);

        // EOF reached with nothing on the line — stop the loop.
        if (line.empty() && input.eof()) {
            break;
        }

        // Skip blank / whitespace-only lines silently.
        if (line.find_first_not_of(" \t") == std::string::npos) {
            continue;
        }

        // In interactive mode errors go through guardOut (same stream as the
        // prompt) to prevent std::cout / std::cerr interleaving that would
        // cause the next prompt '$' to appear on the same line as the error.
        // In batch mode errors go to defaultErr as specified.
        std::ostream& errorOut = interactive ? guardOut : defaultErr;

        try {
            executeLine(line, guardOut, errorOut);
        } catch (const CLIException& ex) {
            formatError(line, ex, errorOut);
            errorOut.flush();
        }

        // Guarantee that the next command (or the next prompt) always starts
        // on a fresh line, regardless of whether the command ended with '\n'.
        // This applies to both interactive and batch mode.
        guardBuf.ensureNewline();

        // Always clear stdin's EOF flag after each command, regardless of mode.
        // Commands without arguments read from std::cin (the console) even
        // during batch execution (per spec). If such a command consumed stdin
        // until EOF (e.g. wc -c, echo), subsequent commands would also see EOF
        // immediately and produce wrong results (e.g. wc -w returning 0).
        std::cin.clear();
    }

    // Restore the previous promptVisible state (important for nested batch calls).
    promptVisible = prevPromptVisible;
}

// ---------------------------------------------------------------------------
// Core pipeline execution
// ---------------------------------------------------------------------------

void Interpreter::executeLine(const std::string& line,
                               std::ostream& defaultOut,
                               std::ostream& defaultErr)
{
    // Step 1: Lex.
    auto tokens = Lexer::tokenize(line);
    if (tokens.empty()) return;

    // Step 2: Parse into a pipeline descriptor.
    PipelineInfo pipeline = CommandLineParser::parse(tokens);
    if (pipeline.empty()) return;

    const int stageCount = static_cast<int>(pipeline.size());

    // Step 3: Instantiate Command objects (constructor validates per-command syntax).
    std::vector<std::unique_ptr<Command>> commands;
    commands.reserve(stageCount);
    for (const CommandInfo& info : pipeline) {
        commands.push_back(CommandFactory::create(info, *this));
    }

    // Step 4: Pipeline-level semantic validation.
    for (int i = 0; i < stageCount; ++i) {
        const Command*     cmd  = commands[i].get();
        const CommandInfo& info = pipeline[i];

        // A command without an input stream may only be the first stage.
        if (!cmd->hasInputStream() && i > 0) {
            throw SemanticException(
                "'" + info.name + "' has no input stream and cannot follow a pipe");
        }

        // A command without an output stream may only be the last stage.
        if (!cmd->hasOutputStream() && i < stageCount - 1) {
            throw SemanticException(
                "'" + info.name + "' has no output stream and cannot be followed by a pipe");
        }

        // A piped-in command may not also specify an input source argument.
        if (i > 0 && cmd->hasInputSourceArgument()) {
            throw SemanticException(
                "'" + info.name + "' receives piped input and cannot also have an input argument");
        }

        // A piped-in command may not also redirect its input.
        if (i > 0 && !info.inputRedirect.empty()) {
            throw SemanticException(
                "'" + info.name + "' receives piped input and cannot also use '<' redirection");
        }

        // A piped-out command may not also redirect its output.
        if (i < stageCount - 1 && !info.outputRedirect.empty()) {
            throw SemanticException(
                "'" + info.name + "' sends output to a pipe and cannot also use '>' redirection");
        }

        // An input source argument and '<' redirection are mutually exclusive.
        if (cmd->hasInputSourceArgument() && !info.inputRedirect.empty()) {
            throw SemanticException(
                "'" + info.name +
                "' has an input source argument and cannot also use '<' redirection");
        }
    }

    // Step 5: Allocate one stringstream pipe-buffer per adjacent stage pair.
    //         pipeBuffers[i] connects stage i (writer) to stage i+1 (reader).
    std::vector<std::unique_ptr<std::stringstream>> pipeBuffers;
    pipeBuffers.reserve(stageCount > 1 ? stageCount - 1 : 0);
    for (int i = 0; i < stageCount - 1; ++i) {
        pipeBuffers.push_back(std::make_unique<std::stringstream>());
    }

    // Step 6: Execute stages left-to-right.
    for (int i = 0; i < stageCount; ++i) {
        Command*           cmd  = commands[i].get();
        const CommandInfo& info = pipeline[i];

        // ---- Resolve INPUT stream ----
        std::unique_ptr<std::ifstream>    inputFile;
        std::unique_ptr<std::istringstream> inlineStream;
        std::istream* inStream = nullptr;

        if (i > 0) {
            // Read from previous stage's pipe buffer (rewind first).
            pipeBuffers[i - 1]->seekg(0);
            inStream = pipeBuffers[i - 1].get();

        } else if (!info.inputRedirect.empty()) {
            // '<' file redirection.
            inputFile = std::make_unique<std::ifstream>(info.inputRedirect);
            if (!*inputFile) {
                throw OSException("cannot open input file: " + info.inputRedirect);
            }
            inStream = inputFile.get();

        } else if (cmd->hasInputSourceArgument()) {
            const Token* srcTok = cmd->getInputSourceArgument();

            if (srcTok->type == TokenType::QUOTED_STRING) {
                // Inline quoted text.
                inlineStream = std::make_unique<std::istringstream>(srcTok->value);
                inStream     = inlineStream.get();
            } else {
                // Unquoted word = filename.
                inputFile = std::make_unique<std::ifstream>(srcTok->value);
                if (!*inputFile) {
                    throw OSException("cannot open input file: " + srcTok->value);
                }
                inStream = inputFile.get();
            }

        } else {
            // Default: read from console.
            inStream = &std::cin;
        }

        // ---- Resolve OUTPUT stream ----
        std::unique_ptr<std::ofstream> outputFile;
        std::ostream* outStream = nullptr;

        if (i < stageCount - 1) {
            // Write into the next stage's pipe buffer.
            outStream = pipeBuffers[i].get();

        } else if (!info.outputRedirect.empty()) {
            // '>' or '>>' file redirection.
            auto mode = std::ios::out |
                        (info.appendMode ? std::ios::app : std::ios::trunc);
            outputFile = std::make_unique<std::ofstream>(info.outputRedirect, mode);
            if (!*outputFile) {
                throw OSException("cannot open output file: " + info.outputRedirect);
            }
            outStream = outputFile.get();

        } else {
            // Default: write to defaultOut.
            outStream = &defaultOut;
        }

        // ---- Run ----
        cmd->execute(*inStream, *outStream, defaultErr);
    }
}

// ---------------------------------------------------------------------------
// Error formatting with caret indicator
// ---------------------------------------------------------------------------

void Interpreter::formatError(const std::string& /*line*/,
                               const CLIException& ex,
                               std::ostream& out) const
{
    const int pos = ex.getPosition();

    if (pos >= 0) {
        // In interactive mode the prompt + space are already printed on the
        // current line, so the caret must be indented by that extra width too.
        // In batch mode no prompt is shown, so only the error position matters.
        const int offset = promptVisible
                         ? static_cast<int>(prompt.length()) + 1 + pos
                         : pos;
        out << std::string(static_cast<std::size_t>(offset), ' ') << '^' << '\n';
    }

    out << ex.what() << '\n';
}
