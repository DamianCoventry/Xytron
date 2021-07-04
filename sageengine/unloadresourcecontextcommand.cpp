#include "unloadresourcecontextcommand.h"
#include "iworld.h"

void UnloadResourceContextCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    Util::ResourceContext* res_cxt = world->GetResourceContext(res_cxt_name_);
    if(res_cxt == NULL)
    {
        throw std::runtime_error(std::string("Can't unload the resource context named [") + res_cxt_name_ + "], because no such resource context exists within the game");
    }

    res_cxt->UnloadAll();
}




UnloadResourceContextCommand::UnloadResourceContextCommand(IWorld* world, MyTest::ResourceContextDocumentMap* res_cxt_map)
: Command(world)
, res_cxt_map_(res_cxt_map)
{
}

CommandInvokeContextPtr UnloadResourceContextCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the resource context to unload

    if(command_line.size() < 2)
    {
        throw std::runtime_error("Insufficient number of command line arguments for UnloadResourceContextCommand");
    }

    return CommandInvokeContextPtr(new UnloadResourceContextCommandInvokeContext(shared_from_this(), command_line[1]));
}
