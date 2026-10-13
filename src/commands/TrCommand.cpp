#include "TrCommand.h"
#include "../exceptions/SyntaxException.h"

// ---------------------------------------------------------------------------
// Construction / argument parsing
// ---------------------------------------------------------------------------

TrCommand::TrCommand(const std::vector<Token>& args) {
    // We need to find the token whose WORD value starts with '-' and whose
    // remainder (after the '-') is the search string OR the entire token is
    // just "-" followed immediately by a QUOTED_STRING token.
    //
    // According to the spec the format is:  tr [argument] -"what" ["with"]
    // The lexer strips quotes, so after lexing "-\"hello\"" the token stream
    // is: WORD("-") then QUOTED_STRING("hello")  OR, if typed without space,
    // just WORD("-hello") — but since '"' is a special char in the lexer,
    // the token right after '-' is actually a QUOTED_STRING.  Therefore the
    // canonical token sequence is:
    //   [input-source]  WORD("-")  QUOTED_STRING(what)  [QUOTED_STRING(with)]
    //
    // We locate the WORD("-") marker, everything before it is input-source,
    // everything after is replacement args.

    int dashIdx = -1;
    for (int i = 0; i < static_cast<int>(args.size()); ++i) {
        if (args[i].type == TokenType::WORD && args[i].value == "-") {
            dashIdx = i;
            break;
        }
    }

    if (dashIdx < 0) {
        throw SyntaxException("tr: missing search pattern; expected -\"what\"");
    }

    // Input-source argument: must be exactly 0 or 1 token before the dash.
    if (dashIdx == 1) {
        inputArg = args[0];
    } else if (dashIdx > 1) {
        throw SyntaxException("tr: too many tokens before search pattern");
    }

    // The token immediately after '-' must be the QUOTED_STRING search value.
    int afterDash = dashIdx + 1;
    if (afterDash >= static_cast<int>(args.size()) ||
        args[afterDash].type != TokenType::QUOTED_STRING)
    {
        throw SyntaxException("tr: expected quoted search string after '-'");
    }
    searchStr = args[afterDash].value;

    // Optional replacement string.
    int withIdx = afterDash + 1;
    if (withIdx < static_cast<int>(args.size())) {
        if (args[withIdx].type != TokenType::QUOTED_STRING) {
            throw SyntaxException("tr: replacement must be a quoted string");
        }
        replaceStr = args[withIdx].value;
        if (withIdx + 1 < static_cast<int>(args.size())) {
            throw SyntaxException("tr: unexpected tokens after replacement string");
        }
    }
    // If withIdx is out of range, replaceStr stays empty → delete mode.
}

// ---------------------------------------------------------------------------
// Private helper
// ---------------------------------------------------------------------------

std::string TrCommand::replaceAll(const std::string& text,
                                  const std::string& from,
                                  const std::string& to)
{
    if (from.empty()) return text;

    std::string result;
    result.reserve(text.size());
    std::size_t pos = 0;

    while (pos < text.size()) {
        std::size_t found = text.find(from, pos);
        if (found == std::string::npos) {
            result.append(text, pos, std::string::npos);
            break;
        }
        result.append(text, pos, found - pos);
        result.append(to);
        pos = found + from.size();
    }

    return result;
}

// ---------------------------------------------------------------------------
// Execution
// ---------------------------------------------------------------------------

void TrCommand::execute(std::istream& in, std::ostream& out, std::ostream& /*err*/) {
    // Read entire input into a string, then perform replacement.
    // Braces avoid the "most vexing parse" ambiguity with iterator constructors.
    std::string content{std::istreambuf_iterator<char>(in),
                        std::istreambuf_iterator<char>()};

    out << replaceAll(content, searchStr, replaceStr);
}

// ---------------------------------------------------------------------------
// Input-source argument accessors
// ---------------------------------------------------------------------------

bool TrCommand::hasInputSourceArgument() const noexcept {
    return inputArg.has_value();
}

const Token* TrCommand::getInputSourceArgument() const noexcept {
    return inputArg.has_value() ? &(*inputArg) : nullptr;
}
