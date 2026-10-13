#include "Lexer.h"
#include "../exceptions/LexicalException.h"

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

bool Lexer::isWhitespace(char c) noexcept {
    return c == ' ' || c == '\t';
}

bool Lexer::isSpecialChar(char c) noexcept {
    return c == '"' || c == '|' || c == '<' || c == '>';
}

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

std::vector<Token> Lexer::tokenize(const std::string& line) {
    std::vector<Token> tokens;
    int i = 0;
    const int n = static_cast<int>(line.size());

    while (i < n) {

        // --- Skip whitespace ---
        if (isWhitespace(line[i])) {
            ++i;
            continue;
        }

        // --- Quoted string ---
        if (line[i] == '"') {
            const int startPos = i;
            ++i; // consume opening '"'

            std::string value;
            while (i < n && line[i] != '"') {
                // Newline inside a quoted string is forbidden by spec
                if (line[i] == '\n') {
                    throw LexicalException(i, "unexpected newline inside quoted string");
                }
                value += line[i++];
            }

            if (i >= n) {
                throw LexicalException(startPos, "unterminated quoted string");
            }

            ++i; // consume closing '"'
            tokens.emplace_back(TokenType::QUOTED_STRING, std::move(value), startPos);
            continue;
        }

        // --- Append redirect '>>' ---
        if (line[i] == '>' && i + 1 < n && line[i + 1] == '>') {
            tokens.emplace_back(TokenType::REDIRECT_APPEND, ">>", i);
            i += 2;
            continue;
        }

        // --- Overwrite redirect '>' ---
        if (line[i] == '>') {
            tokens.emplace_back(TokenType::REDIRECT_OUT, ">", i);
            ++i;
            continue;
        }

        // --- Input redirect '<' ---
        if (line[i] == '<') {
            tokens.emplace_back(TokenType::REDIRECT_IN, "<", i);
            ++i;
            continue;
        }

        // --- Pipe '|' ---
        if (line[i] == '|') {
            tokens.emplace_back(TokenType::PIPE, "|", i);
            ++i;
            continue;
        }

        // --- Unquoted WORD ---
        {
            const int startPos = i;
            std::string value;
            while (i < n && !isWhitespace(line[i]) && !isSpecialChar(line[i])) {
                value += line[i++];
            }
            tokens.emplace_back(TokenType::WORD, std::move(value), startPos);
        }
    }

    return tokens;
}
