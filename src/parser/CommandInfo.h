#pragma once

#include "../lexer/Token.h"
#include <string>
#include <vector>

/// Holds all parsed information for a single command inside a pipeline.
/// The parser fills one CommandInfo per pipeline stage; the interpreter then
/// uses it to create Command objects and wire up their I/O streams.
struct CommandInfo {

    /// The command name as it appeared in the input (e.g. "wc", "echo").
    std::string name;

    /// 0-based position of the first character of the command name in the line.
    /// Used for error-message accuracy.
    int namePosition = 0;

    /// All argument tokens that follow the command name and are NOT redirection
    /// operators or their associated filenames.  Each concrete Command subclass
    /// is responsible for further parsing this list in its constructor.
    std::vector<Token> args;

    /// Filename for input redirection ('<').  Empty string means no redirect.
    std::string inputRedirect;

    /// Filename for output redirection ('>' or '>>').  Empty means no redirect.
    std::string outputRedirect;

    /// True when the output redirect uses append mode ('>>'); false for overwrite ('>').
    bool appendMode = false;
};

/// An ordered sequence of CommandInfo objects representing one full pipeline.
/// Element 0 is the leftmost command; the last element is the rightmost.
using PipelineInfo = std::vector<CommandInfo>;
