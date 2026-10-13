#pragma once

#include "Command.h"
#include "../interpreter/InterpreterContext.h"
#include "../lexer/Token.h"
#include <string>
#include <vector>

/// Implements the 'batch' command.
///
/// Specification: batch filename
/// Reads the named file line-by-line and executes each line as a command
/// through the interpreter, using the batch command's own output stream as
/// the default output for sub-commands.  Errors on individual lines are
/// written to the error stream and execution continues.  The filename must
/// not be quoted (it is a plain WORD token — a filesystem path).
/// 'batch' has no input stream of its own.
class BatchCommand : public Command {
public:

    /// Parses and validates the argument list for 'batch'.
    /// Exactly one WORD argument (the filename) is required.
    /// @param args     Argument tokens from the parsed command line.
    /// @param context  Reference to the interpreter used during execute().
    /// @throws SyntaxException if the argument count or type is wrong.
    BatchCommand(const std::vector<Token>& args, InterpreterContext& context);

    /// Opens the file and executes each line through the interpreter context.
    /// @throws OSException if the file cannot be opened.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// 'batch' does not consume an input stream.
    bool hasInputStream() const noexcept override { return false; }

private:

    /// Path to the batch file to execute.
    std::string filename;

    /// Non-owning reference to the interpreter for executing sub-commands.
    InterpreterContext& context;
};
