#include "DateCommand.h"
#include "../exceptions/SyntaxException.h"
#include <ctime>
#include <iomanip>

DateCommand::DateCommand(const std::vector<Token>& args) {
    if (!args.empty()) {
        throw SyntaxException("date takes no arguments");
    }
}

void DateCommand::execute(std::istream& /*in*/, std::ostream& out, std::ostream& /*err*/) {
    std::time_t now = std::time(nullptr);
    std::tm* local  = std::localtime(&now);

    out << (1900 + local->tm_year)                   << '-'
        << std::setw(2) << std::setfill('0') << (1 + local->tm_mon) << '-'
        << std::setw(2) << std::setfill('0') << local->tm_mday        << '\n';
}
