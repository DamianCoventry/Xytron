#include "loadentitycommand.h"
#include "iworld.h"
#include "../sagedocuments/cellmapdocument.h"

void LoadEntityCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    //Util::ResourceContext* res_cxt = world->GetResourceContext(res_cxt_name_);
    //if(res_cxt == NULL)
    //{
    //    throw std::runtime_error(std::string("Can't Load the image animation [") + name_ + "] from the resource context named [" + res_cxt_name_ + "], because no such resource context exists within the game");
    //}

    //res_cxt->LoadEntity(name_);
}




LoadEntityCommand::LoadEntityCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr LoadEntityCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the image animation to Load
    // command_line[2] is the resource context

    if(command_line.size() < 3)
    {
        throw std::runtime_error("Insufficient number of command line arguments for LoadEntityCommand");
    }

    return CommandInvokeContextPtr(new LoadEntityCommandInvokeContext(shared_from_this(), command_line[2], command_line[1]));
}
