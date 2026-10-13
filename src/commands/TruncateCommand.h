#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <string>
#include <vector>

/// Implements the 'truncate' command.
///
/// Specification: truncate filename
/// Erases all content of the named file without deleting it.
/// If the file does not exist, an OS error is reported.
/// 'truncate' has no input or output stream of its own.
class TruncateCommand : public Command {
public:

    /// Parses and validates the argument list for 'truncate'.
    /// Exactly one WORD argument (the filename) is required.
    /// @throws SyntaxException if the argument count or type is wrong.
    explicit TruncateCommand(const std::vector<Token>& args);

    /// Truncates the file to zero length.
    /// @throws OSException if the file cannot be opened for writing.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// 'truncate' does not consume an input stream.
    bool hasInputStream()  const noexcept override { return false; }

    /// 'truncate' does not produce an output stream.
    bool hasOutputStream() const noexcept override { return false; }

private:

    /// Name of the file to truncate.
    std::string filename;
};
