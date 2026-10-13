#pragma once

#include "Command.h"
#include "../parser/CommandInfo.h"
#include "../interpreter/InterpreterContext.h"
#include <memory>

/// Factory that creates the concrete Command object matching a given CommandInfo.
/// This is the single point of extension for adding new commands — a new branch
/// in create() is all that is required.
///
/// All methods are static; CommandFactory holds no instance state.
class CommandFactory {
public:

    /// Creates and returns the appropriate Command for the given CommandInfo.
    /// @param info    Parsed data for one pipeline stage.
    /// @param context Interpreter context (needed by 'prompt' and 'batch').
    /// @return A heap-allocated Command managed via unique_ptr.
    /// @throws UnknownCommandException if the command name is not recognised.
    /// @throws SyntaxException         if the arguments are invalid for the command.
    static std::unique_ptr<Command> create(const CommandInfo& info,
                                           InterpreterContext& context);
};
