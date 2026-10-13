#include "HeadCommand.h"
#include "../exceptions/SyntaxException.h"
#include <cctype>
#include <stdexcept>

HeadCommand::HeadCommand(const std::vector<Token>& args) {
    if (args.empty()) {
        throw SyntaxException("head requires the -n<count> option");
    }

    // First token must be a WORD matching -n[0-9]{1,5}.
    if (args[0].type != TokenType::WORD) {
        throw SyntaxException("head: first token must be the -n<count> option");
    }

    const std::string& opt = args[0].value;
    if (opt.size() < 3 || opt[0] != '-' || opt[1] != 'n') {
        throw SyntaxException("head: option must be in the form -n<count> (e.g. -n10)");
    }

    // Parse the digit sequence after "-n" — up to 5 digits.
    const std::string digits = opt.substr(2);
    if (digits.empty() || digits.size() > 5) {
        throw SyntaxException("head: -n requires 1 to 5 digits (e.g. -n10)");
    }
    for (char c : digits) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw SyntaxException("head: non-digit character in -n value: '" + opt + "'");
        }
    }

    try {
        lineCount = std::stoi(digits);
    } catch (const std::out_of_range&) {
        throw SyntaxException("head: -n value is out of range");
    }

    // Optional input-source argument.
    if (args.size() == 2) {
        inputArg = args[1];
    } else if (args.size() > 2) {
        throw SyntaxException("head: too many arguments");
    }
}

void HeadCommand::execute(std::istream& in, std::ostream& out, std::ostream& /*err*/) {
    // Read ALL input until EOF first, then output only the first lineCount lines.
    // This ensures that when reading from stdin interactively, output appears
    // only after the user signals EOF (CTRL+Z / CTRL+D), not after the N-th line.
    std::vector<std::string> collected;
    std::string line;

    while (std::getline(in, line)) {
        if (static_cast<int>(collected.size()) < lineCount) {
            collected.push_back(line);
        }
        // Lines beyond lineCount are read and discarded (spec: "ignoriše").
    }

    for (const std::string& l : collected) {
        out << l << '\n';
    }
}

bool HeadCommand::hasInputSourceArgument() const noexcept {
    return inputArg.has_value();
}

const Token* HeadCommand::getInputSourceArgument() const noexcept {
    return inputArg.has_value() ? &(*inputArg) : nullptr;
}
