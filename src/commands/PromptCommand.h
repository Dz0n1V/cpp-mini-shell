#pragma once

#include "Command.h"
#include "../interpreter/InterpreterContext.h"
#include "../lexer/Token.h"
#include <string>
#include <vector>

/// Implements the 'prompt' command.
///
/// Specification: prompt argument
/// Changes the interpreter's command prompt to the string given as a
/// QUOTED_STRING argument.  The argument is mandatory and must be enclosed
/// in double quotes.  'prompt' has no input or output stream of its own.
class PromptCommand : public Command {
public:

    /// Parses and validates the argument list for 'prompt'.
    /// Exactly one QUOTED_STRING argument is required.
    /// @param args     Argument tokens from the parsed command line.
    /// @param context  Reference to the interpreter context used during execute().
    /// @throws SyntaxException if the argument count or type is wrong.
    PromptCommand(const std::vector<Token>& args, InterpreterContext& context);

    /// Calls InterpreterContext::setPrompt() with the stored new-prompt string.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// 'prompt' does not consume an input stream.
    bool hasInputStream()  const noexcept override { return false; }

    /// 'prompt' does not produce output to a stream.
    bool hasOutputStream() const noexcept override { return false; }

private:

    /// The new prompt string (content of the quoted argument, quotes stripped).
    std::string newPrompt;

    /// Non-owning reference to the interpreter; used in execute().
    InterpreterContext& context;
};
