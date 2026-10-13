#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <vector>

/// Implements the 'time' command.
///
/// Specification: time
/// Writes the current real-time clock time (HH:MM:SS) to the output stream.
/// 'time' accepts no arguments and has no input stream.
class TimeCommand : public Command {
public:

    /// Validates that no arguments were supplied.
    /// @throws SyntaxException if any argument tokens are present.
    explicit TimeCommand(const std::vector<Token>& args);

    /// Formats and writes the current time to 'out'.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// 'time' does not read from an input stream.
    bool hasInputStream() const noexcept override { return false; }
};
