#pragma once

#include "../lexer/Token.h"
#include <iostream>

/// Abstract base class for every command supported by the CLI interpreter.
///
/// Design contract:
///   - The constructor of each concrete subclass receives the raw argument
///     token list from CommandInfo::args, validates it, and stores whatever
///     options and parameters the command needs.  Any syntax error throws a
///     SyntaxException before execute() is ever called.
///   - execute() is called by the Interpreter once the I/O streams have been
///     fully resolved (pipe buffers, file redirections, or std::cin / std::cout).
///   - Commands that do not consume an input stream (e.g. time, date) override
///     hasInputStream() to return false; similarly for the output stream.
///   - Commands that use an "input source argument" (a quoted inline string or
///     an unquoted filename given as their data input) expose it via
///     hasInputSourceArgument() and getInputSourceArgument() so the Interpreter
///     can open the correct stream before execute() is called.
class Command {
public:

    /// Virtual destructor — enables correct destruction through base-class pointer.
    virtual ~Command() = default;

    // -------------------------------------------------------------------------
    // Core execution
    // -------------------------------------------------------------------------

    /// Executes the command.
    /// @param in  The resolved input character stream.
    /// @param out The resolved output character stream.
    /// @param err The error output stream (typically std::cerr).
    /// @throws ExecutionException on command-specific runtime failures.
    /// @throws OSException        on OS-level failures during execution.
    virtual void execute(std::istream& in, std::ostream& out, std::ostream& err) = 0;

    // -------------------------------------------------------------------------
    // Stream-capability queries (used by the pipeline validator/executor)
    // -------------------------------------------------------------------------

    /// Returns true if this command reads from an input stream.
    /// Override and return false for commands like time and date.
    virtual bool hasInputStream() const noexcept { return true; }

    /// Returns true if this command writes to an output stream.
    /// Override and return false for commands like touch, rm, truncate, prompt.
    virtual bool hasOutputStream() const noexcept { return true; }

    // -------------------------------------------------------------------------
    // Input-source argument (inline string or filename as the data source)
    // -------------------------------------------------------------------------

    /// Returns true if this command has an explicit "input source argument"
    /// (a quoted string with inline content or an unquoted filename) that
    /// determines where its data comes from.
    virtual bool hasInputSourceArgument() const noexcept { return false; }

    /// Returns a pointer to the input-source argument Token, or nullptr if
    /// hasInputSourceArgument() is false.
    /// The pointed-to Token is owned by the concrete command object and remains
    /// valid for the lifetime of that object.
    virtual const Token* getInputSourceArgument() const noexcept { return nullptr; }
};
