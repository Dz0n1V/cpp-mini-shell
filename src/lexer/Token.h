#pragma once

#include <string>

/// Enumerates every distinct type of lexical token the Lexer can produce.
/// The ordering has no significance; types are matched by value.
enum class TokenType {
    WORD,            ///< An unquoted sequence of non-whitespace, non-special characters.
    QUOTED_STRING,   ///< Content between double quotes (quotes stripped from value).
    PIPE,            ///< The pipe operator '|'.
    REDIRECT_IN,     ///< The input-redirect operator '<'.
    REDIRECT_OUT,    ///< The output-redirect (overwrite) operator '>'.
    REDIRECT_APPEND  ///< The output-redirect (append) operator '>>'.
};

/// Represents a single lexical unit (token) produced by the Lexer.
/// Every token carries its type, string value, and its position in the
/// original input line for accurate error reporting.
struct Token {

    /// The category of this token.
    TokenType type;

    /// The string value of this token.
    /// For WORD: the raw characters; for QUOTED_STRING: content without quotes;
    /// for operator tokens (PIPE, REDIRECT_*): the operator characters themselves.
    std::string value;

    /// 0-based index of the first character of this token in the command line.
    int position;

    /// Constructs a Token with all fields initialised.
    Token(TokenType type, std::string value, int position)
        : type(type), value(std::move(value)), position(position) {}
};
