#pragma once

#include "Token.h"
#include <string>
#include <vector>

/// Performs lexical analysis on a single command-line string and converts it
/// into a flat sequence of Token objects.
///
/// Lexical rules (per spec):
///   - Whitespace (space, horizontal tab) separates tokens and is discarded.
///   - '"' ... '"' produces a QUOTED_STRING; newline and '"' are forbidden inside.
///   - '>>' produces REDIRECT_APPEND; '>' produces REDIRECT_OUT.
///   - '<' produces REDIRECT_IN; '|' produces PIPE.
///   - Any other contiguous sequence of non-whitespace, non-special characters
///     produces a WORD token.
///
/// All methods are static; Lexer has no instance state.
class Lexer {
public:

    /// Tokenises the given command-line string (without its trailing '\n').
    /// @param line The raw command-line string to tokenise.
    /// @return A vector of Token objects in left-to-right order.
    /// @throws LexicalException if an unterminated quoted string is found.
    static std::vector<Token> tokenize(const std::string& line);

private:

    /// Returns true if c is a whitespace character (space or horizontal tab).
    static bool isWhitespace(char c) noexcept;

    /// Returns true if c is a special character that cannot appear unquoted inside
    /// a WORD token: '"', '|', '<', '>'.
    static bool isSpecialChar(char c) noexcept;
};
