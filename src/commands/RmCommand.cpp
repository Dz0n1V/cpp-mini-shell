#include "RmCommand.h"
#include "../exceptions/SyntaxException.h"
#include "../exceptions/OSException.h"
#include <filesystem>

RmCommand::RmCommand(const std::vector<Token>& args) {
    if (args.size() != 1 || args[0].type != TokenType::WORD) {
        throw SyntaxException("rm requires exactly one filename argument");
    }
    filename = args[0].value;
}

void RmCommand::execute(std::istream& /*in*/, std::ostream& /*out*/, std::ostream& /*err*/) {
    std::error_code ec;
    bool removed = std::filesystem::remove(filename, ec);

    if (!removed || ec) {
        throw OSException("cannot remove file '" + filename + "': " +
                          (ec ? ec.message() : "file not found"));
    }
}
