#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <string>
#include <vector>

/// Implements the 'touch' command.
///
/// Specification: touch filename
/// Creates an empty file with the given name in the current working directory.
/// If the file already exists, an error is reported and no change is made.
/// 'touch' has no input or output stream of its own.
class TouchCommand : public Command {
public:

    /// Parses and validates the argument list for 'touch'.
    /// Exactly one WORD argument (the filename) is required.
    /// @throws SyntaxException if the argument count or type is wrong.
    explicit TouchCommand(const std::vector<Token>& args);

    /// Creates the file; throws if it already exists.
    /// @throws ExecutionException if the file already exists.
    /// @throws OSException        if the OS fails to create the file.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// 'touch' does not consume an input stream.
    bool hasInputStream()  const noexcept override { return false; }

    /// 'touch' does not produce an output stream.
    bool hasOutputStream() const noexcept override { return false; }

private:

    /// Name of the file to create.
    std::string filename;
};
