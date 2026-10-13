#pragma once

#include "CLIException.h"

/// Exception thrown when the interpreter encounters a command name that is
/// not registered in the command factory.
/// Per the specification, the message must read: "Unknown command: <n>".
class UnknownCommandException : public CLIException {

    /// The unrecognized command name that triggered this exception.
    std::string commandName;

    /// 0-based position of the command name in the input line.
    int position;

public:

    /// Constructs an UnknownCommandException for the given command name and position.
    /// @param name The unrecognized command name as it appeared in the input.
    /// @param pos  0-based index of the first character of the command name.
    UnknownCommandException(const std::string& name, int pos)
        : CLIException("Unknown command: " + name), commandName(name), position(pos) {}

    /// Returns the unrecognized command name.
    const std::string& getCommandName() const noexcept { return commandName; }

    /// Returns the 0-based position of the command name in the input line.
    int getPosition() const noexcept override { return position; }
};
