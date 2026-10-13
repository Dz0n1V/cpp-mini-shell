#include "PromptCommand.h"
#include "../exceptions/SyntaxException.h"

PromptCommand::PromptCommand(const std::vector<Token>& args, InterpreterContext& ctx)
    : context(ctx)
{
    if (args.size() != 1 || args[0].type != TokenType::QUOTED_STRING) {
        throw SyntaxException("prompt requires exactly one quoted-string argument");
    }
    newPrompt = args[0].value;
}

void PromptCommand::execute(std::istream& /*in*/, std::ostream& /*out*/, std::ostream& /*err*/) {
    context.setPrompt(newPrompt);
}
