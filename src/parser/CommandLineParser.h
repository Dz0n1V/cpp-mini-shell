#pragma once

#include "CommandInfo.h"
#include "../lexer/Token.h"
#include <vector>

/// Converts a flat token list (produced by the Lexer) into a PipelineInfo —
/// an ordered sequence of CommandInfo objects, one per pipeline stage.
///
/// Responsibilities:
///   - Split the token list on PIPE tokens to isolate pipeline segments.
///   - For each segment, identify the command name (must be the first WORD).
///   - Collect REDIRECT_IN / REDIRECT_OUT / REDIRECT_APPEND operators together
///     with the filename token that immediately follows each one.
///   - Enforce that redirection operators appear only at the end of a segment
///     and that no duplicate redirections exist for the same direction.
///
/// All methods are static; CommandLineParser has no instance state.
class CommandLineParser {
public:

    /// Parses a token list into a PipelineInfo.
    /// @param tokens The token list produced by Lexer::tokenize().
    /// @return A PipelineInfo with one CommandInfo per pipeline stage.
    ///         Returns an empty PipelineInfo if the token list is empty.
    /// @throws SyntaxException   on structural syntax violations.
    /// @throws SemanticException on duplicate redirections.
    static PipelineInfo parse(const std::vector<Token>& tokens);

private:

    /// Parses one pipeline segment (the tokens between two consecutive PIPE
    /// tokens, or between a PIPE and an end of the token list) into a CommandInfo.
    /// @param tokens Tokens belonging exclusively to one pipeline stage.
    /// @throws SyntaxException   if the command name is missing or malformed.
    /// @throws SemanticException if a redirection appears more than once.
    static CommandInfo parseSegment(const std::vector<Token>& tokens);
};
