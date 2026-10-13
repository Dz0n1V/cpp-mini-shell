#pragma once

#include <exception>
#include <string>

/// Base class for all CLI interpreter exceptions.
/// All custom exception types derive from this class, which itself derives
/// from std::exception to comply with the C++ standard exception hierarchy.
class CLIException : public std::exception {
protected:

    /// The human-readable error message returned by what().
    std::string message;

public:

    /// Constructs a CLIException with the given error message.
    explicit CLIException(std::string msg) : message(std::move(msg)) {}

    /// Returns the null-terminated error message string.
    const char* what() const noexcept override { return message.c_str(); }

    /// Returns the 0-based position of the error in the command line, or -1
    /// if this exception type does not carry position information.
    /// Subclasses that know the position override this method.
    virtual int getPosition() const noexcept { return -1; }
};
