#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <optional>
#include <vector>

/// Implements the 'echo' command.
///
/// Specification: echo [argument]
/// Copies every character from its input stream to its output stream
/// without modification.  The optional argument defines the input source:
///   - QUOTED_STRING  →  the string content becomes the input stream.
///   - WORD           →  treated as a filename; file content is the input.
///   - (absent)       →  reads from stdin (or from the pipe in a pipeline).
class EchoCommand : public Command {
public:

    /// Parses and validates the argument list for 'echo'.
    /// Accepts zero or one argument token (WORD or QUOTED_STRING).
    /// @throws SyntaxException if more than one argument is supplied.
    explicit EchoCommand(const std::vector<Token>& args);

    /// Copies all characters from 'in' to 'out' verbatim.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// Returns true when an input source argument was provided.
    bool hasInputSourceArgument() const noexcept override;

    /// Returns a pointer to the input source token, or nullptr if absent.
    const Token* getInputSourceArgument() const noexcept override;

private:

    /// The optional input-source argument token (WORD filename or QUOTED_STRING).
    std::optional<Token> inputArg;
};
