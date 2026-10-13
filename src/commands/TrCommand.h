#pragma once

#include "Command.h"
#include "../lexer/Token.h"
#include <optional>
#include <string>
#include <vector>

/// Implements the 'tr' command.
///
/// Specification: tr [argument] -"what" ["with"]
/// Replaces all occurrences of the string 'what' in the input stream with
/// the string 'with'.  If 'with' is omitted, all occurrences of 'what' are
/// removed.  The search string is introduced by a WORD token that begins with
/// a dash followed immediately (no space) by the quoted search string:
///   e.g.  -"hello"
///
/// Token layout expected in args (after the command name):
///   [input-source-token]  -"what"  ["with-token"]
///
/// The presence of an input-source argument is determined by whether the first
/// WORD/QUOTED_STRING token before the dash-prefixed token is present.
class TrCommand : public Command {
public:

    /// Parses the argument list for 'tr'.
    /// Locates the mandatory dash-prefixed search token, stores the optional
    /// input-source token, and the optional replacement token.
    /// @throws SyntaxException if the search token is absent or malformed.
    explicit TrCommand(const std::vector<Token>& args);

    /// Performs the replacement/removal and writes the result to 'out'.
    void execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    /// Returns true when an explicit input-source argument was supplied.
    bool hasInputSourceArgument() const noexcept override;

    /// Returns a pointer to the input-source token, or nullptr.
    const Token* getInputSourceArgument() const noexcept override;

private:

    /// The substring to search for (from the mandatory -"what" token).
    std::string searchStr;

    /// The replacement string; empty when 'with' is omitted (delete mode).
    std::string replaceStr;

    /// Optional input-source argument (WORD filename or QUOTED_STRING content).
    std::optional<Token> inputArg;

    /// Replaces all non-overlapping occurrences of 'from' in 'text' with 'to'.
    static std::string replaceAll(const std::string& text,
                                  const std::string& from,
                                  const std::string& to);
};
