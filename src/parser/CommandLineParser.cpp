#include "CommandLineParser.h"
#include "../exceptions/SyntaxException.h"
#include "../exceptions/SemanticException.h"

// ---------------------------------------------------------------------------
// Segment parser
// ---------------------------------------------------------------------------

CommandInfo CommandLineParser::parseSegment(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        throw SyntaxException("empty command segment");
    }

    // The first token must be an unquoted WORD — the command name.
    if (tokens[0].type != TokenType::WORD) {
        throw SyntaxException(
            "expected command name (quoted strings and operators are not valid command names)",
            tokens[0].position);
    }

    CommandInfo info;
    info.name         = tokens[0].value;
    info.namePosition = tokens[0].position;

    int i = 1;
    const int n = static_cast<int>(tokens.size());

    // Track whether we have already started collecting redirections so that
    // we can reject any plain argument tokens that appear after them.
    bool seenRedirect = false;

    while (i < n) {
        const Token& tok = tokens[i];

        if (tok.type == TokenType::REDIRECT_IN) {
            if (!info.inputRedirect.empty()) {
                throw SemanticException("duplicate input redirection for command '" + info.name + "'");
            }
            ++i;
            if (i >= n || tokens[i].type != TokenType::WORD) {
                throw SyntaxException(
                    "expected filename after '<'",
                    tokens[i - 1].position);
            }
            info.inputRedirect = tokens[i].value;
            seenRedirect = true;
            ++i;

        } else if (tok.type == TokenType::REDIRECT_OUT || tok.type == TokenType::REDIRECT_APPEND) {
            if (!info.outputRedirect.empty()) {
                throw SemanticException("duplicate output redirection for command '" + info.name + "'");
            }
            info.appendMode = (tok.type == TokenType::REDIRECT_APPEND);
            ++i;
            if (i >= n || tokens[i].type != TokenType::WORD) {
                throw SyntaxException(
                    "expected filename after redirect operator",
                    tokens[i - 1].position);
            }
            info.outputRedirect = tokens[i].value;
            seenRedirect = true;
            ++i;

        } else {
            // Regular argument token — must not appear after a redirection.
            if (seenRedirect) {
                throw SyntaxException(
                    "argument appears after a redirection operator; "
                    "redirections must be at the end of the command",
                    tok.position);
            }
            info.args.push_back(tok);
            ++i;
        }
    }

    return info;
}

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

PipelineInfo CommandLineParser::parse(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        return {};
    }

    PipelineInfo pipeline;
    std::vector<Token> segment;

    for (const Token& tok : tokens) {
        if (tok.type == TokenType::PIPE) {
            if (segment.empty()) {
                throw SyntaxException(
                    "empty command before pipe '|'",
                    tok.position);
            }
            pipeline.push_back(parseSegment(segment));
            segment.clear();
        } else {
            segment.push_back(tok);
        }
    }

    // Last segment after the final pipe (or the only segment if no pipes).
    if (segment.empty() && !pipeline.empty()) {
        // Trailing pipe — the token list ended right after '|'.
        throw SyntaxException("empty command after last pipe '|'");
    }

    if (!segment.empty()) {
        pipeline.push_back(parseSegment(segment));
    }

    return pipeline;
}
