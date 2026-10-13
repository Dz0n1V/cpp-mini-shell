#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <string>
#include <vector>

/// Implements the 'rm' command.
///
/// Specification: rm filename
/// Removes (permanently deletes) the named file from the filesystem.
/// 'rm' has no input or output stream of its own.
class RmCommand : public Command {
public:

    /// Parses and validates the argument list for 'rm'.
    /// Exactly one WORD argument (the filename) is required.
    /// @throws SyntaxException if the argument count or type is wrong.
    explicit RmCommand(const std::vector<Token>& args);

    /// Deletes the file from the filesystem.
    /// @throws OSException if the file does not exist or cannot be deleted.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// 'rm' does not consume an input stream.
    bool hasInputStream()  const noexcept override { return false; }

    /// 'rm' does not produce an output stream.
    bool hasOutputStream() const noexcept override { return false; }

private:

    /// Name of the file to remove.
    std::string filename;
};
