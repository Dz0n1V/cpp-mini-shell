#pragma once

#include "InterpreterContext.h"
#include "../exceptions/CLIException.h"
#include <iostream>
#include <string>

/// The central controller of the CLI interpreter.
///
/// Implements InterpreterContext so that 'prompt' and 'batch' commands can
/// call back into the running interpreter without depending on this full class.
///
/// Responsibilities:
///   - Read input lines (max 512 chars per spec) from any istream.
///   - Lex, parse, semantically validate, and execute each command line.
///   - Manage the current prompt string.
///   - Build and execute pipelines: resolve pipe buffers, file redirections,
///     and default stdin/stdout for each pipeline stage.
class Interpreter : public InterpreterContext {
public:

    /// Constructs the Interpreter with the default prompt '$'.
    Interpreter();

    /// Enters the interactive REPL loop reading from std::cin.
    /// Returns when stdin reaches EOF.
    void run();

    /// Reads lines from 'input' and executes them one by one.
    /// Used both by run() and by runBatchFile().
    /// @param input      The source stream of command lines.
    /// @param defaultOut Default output stream passed to commands.
    /// @param defaultErr Error stream for reporting per-line failures.
    void runStream(std::istream& input,
                   std::ostream& defaultOut,
                   std::ostream& defaultErr);

    // -------------------------------------------------------------------------
    // InterpreterContext interface
    // -------------------------------------------------------------------------

    /// Changes the command-prompt string to 'newPrompt'.
    void setPrompt(const std::string& newPrompt) override;

    /// Opens 'filename' and feeds its lines to runStream().
    /// @throws OSException if the file cannot be opened.
    void runBatchFile(const std::string& filename,
                      std::ostream& defaultOut,
                      std::ostream& defaultErr) override;

private:

    /// Current prompt string (default '$').
    std::string prompt;

    /// True while a runStream() call is executing in interactive mode (stdin).
    /// Used by formatError() to decide whether to include the prompt width in
    /// the caret offset calculation.
    bool promptVisible = false;

    /// Reads at most 512 chars from 'input', discarding excess up to '\n'.
    /// Returns an empty string on EOF.
    std::string readLine(std::istream& input);

    /// Lexes, parses, validates, and executes one command-line string.
    /// @throws CLIException (and subclasses) on any error.
    void executeLine(const std::string& line,
                     std::ostream& defaultOut,
                     std::ostream& defaultErr);

    /// Formats an error for display: if the exception carries a position,
    /// prints a caret line pointing to that position in the original input,
    /// then prints the error message on the next line.
    /// @param line   The original command-line string (without prompt).
    /// @param ex     The exception to display.
    /// @param out    The stream to write the formatted error to.
    void formatError(const std::string& line,
                     const CLIException& ex,
                     std::ostream& out) const;
};
