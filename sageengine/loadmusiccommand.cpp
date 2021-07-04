#include "loadmusiccommand.h"
#include "iworld.h"

void LoadMusicCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    Util::ResourceContext* res_cxt = world->GetResourceContext(res_cxt_name_);
    if(res_cxt == NULL)
    {
        throw std::runtime_error(std::string("Can't Load the music track [") + filename_ + "] from the resource context named [" + res_cxt_name_ + "], because no such resource context exists within the game");
    }

    res_cxt->LoadMusic(filename_, window_, window_message_id_);
}




LoadMusicCommand::LoadMusicCommand(IWorld* world, HWND window, int window_message_id)
: Command(world)
, window_(window)
, window_message_id_(window_message_id)
{
}

CommandInvokeContextPtr LoadMusicCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the music to Load
    // command_line[2] is the name of the resource context

    if(command_line.size() < 3)
    {
        throw std::runtime_error("Insufficient number of command line arguments for LoadMusicCommand");
    }

    return CommandInvokeContextPtr(new LoadMusicCommandInvokeContext(shared_from_this(), command_line[2], command_line[1], window_, window_message_id_));
}
