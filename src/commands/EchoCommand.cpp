#include "EchoCommand.h"
#include "../exceptions/SyntaxException.h"

EchoCommand::EchoCommand(const std::vector<Token>& args) {
    if (args.size() > 1) {
        throw SyntaxException("echo accepts at most one argument");
    }
    if (!args.empty()) {
        inputArg = args[0];
    }
}

void EchoCommand::execute(std::istream& in, std::ostream& out, std::ostream& /*err*/) {
    // Read the entire input into a buffer first, then write it all at once.
    // This ensures that when reading from stdin interactively, output appears
    // only after the user signals EOF (CTRL+Z / CTRL+D), not line by line.
    std::string content{std::istreambuf_iterator<char>(in),
                        std::istreambuf_iterator<char>()};
    out << content;
}

bool EchoCommand::hasInputSourceArgument() const noexcept {
    return inputArg.has_value();
}

const Token* EchoCommand::getInputSourceArgument() const noexcept {
    return inputArg.has_value() ? &(*inputArg) : nullptr;
}
