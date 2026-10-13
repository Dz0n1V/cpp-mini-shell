#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <optional>
#include <vector>

/// Enumerates the counting modes supported by 'wc'.
enum class WcMode {
    WORDS,      ///< Count whitespace-separated words (-w).
    CHARACTERS  ///< Count every character (-c).
};

/// Implements the 'wc' command.
///
/// Specification: wc -opt [argument]
/// Counts words (-w) or all characters (-c) in the input stream and writes
/// the count to the output stream.  The option flag is mandatory.
/// The optional argument defines the input source identically to 'echo'.
class WcCommand : public Command {
public:

    /// Parses and validates the argument list for 'wc'.
    /// The first token must be the option (-w or -c); at most one further
    /// input-source token may follow.
    /// @throws SyntaxException if the option is missing, unrecognised, or
    ///         more than one argument token follows the option.
    explicit WcCommand(const std::vector<Token>& args);

    /// Counts tokens or characters in 'in' and writes the result to 'out'.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// Returns true when an explicit input-source argument was provided.
    bool hasInputSourceArgument() const noexcept override;

    /// Returns a pointer to the input-source argument token, or nullptr.
    const Token* getInputSourceArgument() const noexcept override;

private:

    /// The counting mode determined by the -w / -c option.
    WcMode mode;

    /// Optional input-source argument token (WORD filename or QUOTED_STRING).
    std::optional<Token> inputArg;

    /// Counts words (sequences of non-whitespace characters).
    static long long countWords(std::istream& in);

    /// Counts every character in the stream.
    static long long countCharacters(std::istream& in);
};
