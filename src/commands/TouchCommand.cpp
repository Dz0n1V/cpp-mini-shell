#include "TouchCommand.h"
#include "../exceptions/SyntaxException.h"
#include "../exceptions/ExecutionException.h"
#include "../exceptions/OSException.h"
#include <fstream>
#include <filesystem>

TouchCommand::TouchCommand(const std::vector<Token>& args) {
    if (args.size() != 1 || args[0].type != TokenType::WORD) {
        throw SyntaxException("touch requires exactly one filename argument");
    }
    filename = args[0].value;
}

void TouchCommand::execute(std::istream& /*in*/, std::ostream& /*out*/, std::ostream& /*err*/) {
    std::error_code ec;
    const bool alreadyExists = std::filesystem::exists(filename, ec);
    if (ec) {
        throw OSException("cannot access '" + filename + "': " + ec.message());
    }
    if (alreadyExists) {
        throw ExecutionException("file already exists: " + filename);
    }

    std::ofstream ofs(filename);
    if (!ofs) {
        throw OSException("cannot create file: " + filename);
    }
}
