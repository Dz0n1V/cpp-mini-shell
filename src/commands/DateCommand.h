#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <vector>

/// Implements the 'date' command.
///
/// Specification: date
/// Writes the current real-time clock date (YYYY-MM-DD) to the output stream.
/// 'date' accepts no arguments and has no input stream.
class DateCommand : public Command {
public:

    /// Validates that no arguments were supplied.
    /// @throws SyntaxException if any argument tokens are present.
    explicit DateCommand(const std::vector<Token>& args);

    /// Formats and writes the current date to 'out'.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// 'date' does not read from an input stream.
    bool hasInputStream() const noexcept override { return false; }
};
