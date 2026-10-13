#pragma once

#include "CLIException.h"

/// Exception thrown when a command fails during its own execution for a
/// reason that is specific to that command's semantics (e.g., touch on an
/// already-existing file).
class ExecutionException : public CLIException {
public:

    /// Constructs an ExecutionException with the given detail message.
    /// @param detail A description of the execution-time failure.
    explicit ExecutionException(const std::string& detail)
        : CLIException("Execution error: " + detail) {}
};
