#pragma once

#include "CLIException.h"

/// Exception thrown when a semantically invalid combination of I/O definitions
/// is detected — for example, a command that has both an input argument and an
/// input redirection, or a command with no input stream placed mid-pipeline.
class SemanticException : public CLIException {
public:

    /// Constructs a SemanticException with the given detail message.
    /// @param detail A description of the semantic conflict.
    explicit SemanticException(const std::string& detail)
        : CLIException("Semantic error: " + detail) {}
};
