#include "BatchCommand.h"
#include "../exceptions/SyntaxException.h"

BatchCommand::BatchCommand(const std::vector<Token>& args, InterpreterContext& ctx)
    : context(ctx)
{
    if (args.size() != 1 || args[0].type != TokenType::WORD) {
        throw SyntaxException("batch requires exactly one filename argument");
    }
    filename = args[0].value;
}

void BatchCommand::execute(std::istream& /*in*/, std::ostream& out, std::ostream& err) {
    context.runBatchFile(filename, out, err);
}
