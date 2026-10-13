#pragma once

#include "CLIException.h"

/// Exception thrown when the parser or a command constructor detects a
/// syntax error — for example, a missing required option, a wrong number
/// of arguments, or an ill-formed command line structure.
class SyntaxException : public CLIException {

    /// 0-based position of the error in the command line, or -1 if unknown.
    int position;

public:

    /// Constructs a SyntaxException with a detail message and no position.
    /// @param detail A description of the specific syntax error.
    explicit SyntaxException(const std::string& detail)
        : CLIException("Syntax error: " + detail), position(-1) {}

    /// Constructs a SyntaxException with a detail message and a known position.
    /// @param detail A description of the specific syntax error.
    /// @param pos    0-based index of the offending character in the command line.
    SyntaxException(const std::string& detail, int pos)
        : CLIException("Syntax error: " + detail), position(pos) {}

    /// Returns the 0-based position of the error, or -1 if unknown.
    int getPosition() const noexcept override { return position; }
};
