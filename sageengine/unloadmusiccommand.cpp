#include "unloadmusiccommand.h"
#include "iworld.h"

void UnloadMusicCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    Util::ResourceContext* res_cxt = world->GetResourceContext(res_cxt_name_);
    if(res_cxt == NULL)
    {
        throw std::runtime_error(std::string("Can't unload the music track [") + filename_ + "] from the resource context named [" + res_cxt_name_ + "], because no such resource context exists within the game");
    }

    res_cxt->UnloadMusic(filename_);
}




UnloadMusicCommand::UnloadMusicCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr UnloadMusicCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the music to unload
    // command_line[2] is the name of the resource context

    if(command_line.size() < 3)
    {
        throw std::runtime_error("Insufficient number of command line arguments for UnloadMusicCommand");
    }

    return CommandInvokeContextPtr(new UnloadMusicCommandInvokeContext(shared_from_this(), command_line[2], command_line[1]));
}
