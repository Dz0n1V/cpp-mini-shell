#include "TimeCommand.h"
#include "../exceptions/SyntaxException.h"
#include <ctime>
#include <iomanip>

TimeCommand::TimeCommand(const std::vector<Token>& args) {
    if (!args.empty()) {
        throw SyntaxException("time takes no arguments");
    }
}

void TimeCommand::execute(std::istream& /*in*/, std::ostream& out, std::ostream& /*err*/) {
    std::time_t now = std::time(nullptr);
    std::tm* local  = std::localtime(&now);

    out << std::setw(2) << std::setfill('0') << local->tm_hour << ':'
        << std::setw(2) << std::setfill('0') << local->tm_min  << ':'
        << std::setw(2) << std::setfill('0') << local->tm_sec  << '\n';
}
