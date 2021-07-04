#include "loadsoundcommand.h"
#include "iworld.h"

void LoadSoundCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    Util::ResourceContext* res_cxt = world->GetResourceContext(res_cxt_name_);
    if(res_cxt == NULL)
    {
        throw std::runtime_error(std::string("Can't Load the sound [") + filename_ + "] from the resource context named [" + res_cxt_name_ + "], because no such resource context exists within the game");
    }

    res_cxt->LoadSound(filename_, 1.0f, 5.0f);        // TODO: Load the 1.0f and 5.0f from the game document
}




LoadSoundCommand::LoadSoundCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr LoadSoundCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the sound to Load
    // command_line[2] is the name of the resource context

    if(command_line.size() < 3)
    {
        throw std::runtime_error("Insufficient number of command line arguments for LoadSoundCommand");
    }

    return CommandInvokeContextPtr(new LoadSoundCommandInvokeContext(shared_from_this(), command_line[2], command_line[1]));
}
