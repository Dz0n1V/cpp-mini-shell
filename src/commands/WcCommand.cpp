#include "WcCommand.h"
#include "../exceptions/SyntaxException.h"
#include <cctype>

WcCommand::WcCommand(const std::vector<Token>& args) {
    if (args.empty()) {
        throw SyntaxException("wc requires an option: -w (words) or -c (characters)");
    }

    // First token must be an option word: "-w" or "-c".
    if (args[0].type != TokenType::WORD) {
        throw SyntaxException("wc: expected -w or -c option");
    }

    const std::string& opt = args[0].value;
    if (opt == "-w") {
        mode = WcMode::WORDS;
    } else if (opt == "-c") {
        mode = WcMode::CHARACTERS;
    } else {
        throw SyntaxException("wc: unknown option '" + opt + "'; use -w or -c");
    }

    // Optional input-source argument.
    if (args.size() == 2) {
        inputArg = args[1];
    } else if (args.size() > 2) {
        throw SyntaxException("wc: too many arguments");
    }
}

long long WcCommand::countWords(std::istream& in) {
    long long count = 0;
    bool inWord = false;
    char ch;
    while (in.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            inWord = false;
        } else {
            if (!inWord) {
                ++count;
                inWord = true;
            }
        }
    }
    return count;
}

long long WcCommand::countCharacters(std::istream& in) {
    long long count = 0;
    char ch;
    while (in.get(ch)) {
        ++count;
    }
    return count;
}

void WcCommand::execute(std::istream& in, std::ostream& out, std::ostream& /*err*/) {
    long long result = (mode == WcMode::WORDS) ? countWords(in) : countCharacters(in);
    out << result << '\n';
}

bool WcCommand::hasInputSourceArgument() const noexcept {
    return inputArg.has_value();
}

const Token* WcCommand::getInputSourceArgument() const noexcept {
    return inputArg.has_value() ? &(*inputArg) : nullptr;
}
