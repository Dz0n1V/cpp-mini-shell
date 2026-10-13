#pragma once

#include "CLIException.h"

/// Exception thrown when an operating-system–level operation fails, such as
/// opening, creating, or deleting a file, or any other system call that
/// returns an error code.
class OSException : public CLIException {
public:

    /// Constructs an OSException with the given detail message.
    /// @param detail A human-readable description of the OS-level failure.
    explicit OSException(const std::string& detail)
        : CLIException("OS error: " + detail) {}
};
