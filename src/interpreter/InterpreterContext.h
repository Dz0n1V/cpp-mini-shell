#pragma once

#include <ostream>
#include <string>

/// Pure-abstract interface that commands needing interpreter-level services
/// (prompt and batch) use to interact with the running Interpreter without
/// depending on the concrete Interpreter class.
///
/// The Interpreter itself implements this interface.
class InterpreterContext {
public:

    /// Changes the command-prompt string displayed before each new command line.
    /// @param newPrompt The new prompt string (already without enclosing quotes).
    virtual void setPrompt(const std::string& newPrompt) = 0;

    /// Reads the given file line-by-line and executes each line as a command,
    /// using defaultOut as the default output stream for sub-commands and
    /// defaultErr as the error stream.  Errors from individual lines are
    /// written to defaultErr; execution continues with the next line.
    /// @param filename   Path to the batch file (relative to the CWD).
    /// @param defaultOut Default output stream for sub-commands.
    /// @param defaultErr Error output stream for sub-commands.
    /// @throws OSException if the file cannot be opened.
    virtual void runBatchFile(const std::string& filename,
                              std::ostream& defaultOut,
                              std::ostream& defaultErr) = 0;

    /// Virtual destructor.
    virtual ~InterpreterContext() = default;
};
