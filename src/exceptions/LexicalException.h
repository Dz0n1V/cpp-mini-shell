#pragma once

#include "CLIException.h"

/// Exception thrown when the lexer encounters an invalid character or
/// character sequence at a specific position in the command line.
/// Reports the 0-based position of the first invalid character found.
class LexicalException : public CLIException {

    /// 0-based index of the first invalid character in the input line.
    int position;

public:

    /// Constructs a LexicalException for the given position and detail message.
    /// @param pos    0-based index of the offending character.
    /// @param detail Additional context describing the lexical error.
    LexicalException(int pos, const std::string& detail)
        : CLIException("Lexical error at position " + std::to_string(pos) + ": " + detail),
          position(pos) {}

    /// Returns the 0-based position of the invalid character.
    int getPosition() const noexcept override { return position; }
};
