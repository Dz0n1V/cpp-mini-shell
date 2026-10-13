#include "CommandFactory.h"
#include "EchoCommand.h"
#include "PromptCommand.h"
#include "TimeCommand.h"
#include "DateCommand.h"
#include "TouchCommand.h"
#include "TruncateCommand.h"
#include "RmCommand.h"
#include "WcCommand.h"
#include "TrCommand.h"
#include "HeadCommand.h"
#include "BatchCommand.h"
#include "../exceptions/UnknownCommandException.h"

std::unique_ptr<Command> CommandFactory::create(const CommandInfo& info,
                                                InterpreterContext& context)
{
    const std::string& name = info.name;
    const auto& args        = info.args;

    if (name == "echo")     return std::make_unique<EchoCommand>(args);
    if (name == "prompt")   return std::make_unique<PromptCommand>(args, context);
    if (name == "time")     return std::make_unique<TimeCommand>(args);
    if (name == "date")     return std::make_unique<DateCommand>(args);
    if (name == "touch")    return std::make_unique<TouchCommand>(args);
    if (name == "truncate") return std::make_unique<TruncateCommand>(args);
    if (name == "rm")       return std::make_unique<RmCommand>(args);
    if (name == "wc")       return std::make_unique<WcCommand>(args);
    if (name == "tr")       return std::make_unique<TrCommand>(args);
    if (name == "head")     return std::make_unique<HeadCommand>(args);
    if (name == "batch")    return std::make_unique<BatchCommand>(args, context);

    throw UnknownCommandException(name, info.namePosition);
}
