#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <optional>
#include <vector>

/// Implements the 'head' command.
///
/// Specification: head -ncount [argument]
/// Copies the first 'count' lines from the input stream to the output stream.
/// The mandatory option is -n followed immediately (no whitespace) by up to
/// five decimal digits, e.g. "-n10".  The optional argument is an input-source
/// token identical in meaning to 'echo' and 'wc'.
class HeadCommand : public Command {
public:

    /// Parses and validates the argument list for 'head'.
    /// The first token must match the pattern -n[0-9]{1,5}.
    /// At most one further input-source token is accepted.
    /// @throws SyntaxException if the -n option is absent, malformed, or the
    ///         argument count is wrong.
    explicit HeadCommand(const std::vector<Token>& args);

    /// Copies the first lineCount lines of 'in' to 'out'.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// Returns true when an explicit input-source argument was supplied.
    bool hasInputSourceArgument() const noexcept override;

    /// Returns a pointer to the input-source token, or nullptr.
    const Token* getInputSourceArgument() const noexcept override;

private:

    /// Number of lines to transfer, parsed from the -nNNN option.
    int lineCount;

    /// Optional input-source argument.
    std::optional<Token> inputArg;
};
