#include "TruncateCommand.h"
#include "../exceptions/SyntaxException.h"
#include "../exceptions/OSException.h"
#include <fstream>
#include <filesystem>

TruncateCommand::TruncateCommand(const std::vector<Token>& args) {
    if (args.size() != 1 || args[0].type != TokenType::WORD) {
        throw SyntaxException("truncate requires exactly one filename argument");
    }
    filename = args[0].value;
}

void TruncateCommand::execute(std::istream& /*in*/, std::ostream& /*out*/, std::ostream& /*err*/) {
    std::error_code ec;
    const bool fileExists = std::filesystem::exists(filename, ec);
    if (ec) {
        throw OSException("cannot access '" + filename + "': " + ec.message());
    }
    if (!fileExists) {
        throw OSException("file not found: " + filename);
    }

    // Open in out|trunc mode to clear the content without deleting the file.
    std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
    if (!ofs) {
        throw OSException("cannot open file for truncation: " + filename);
    }
}
