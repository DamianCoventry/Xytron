#include "currentcamerafirstcommand.h"
#include "iworld.h"

void CurrentCameraFirstCommandInvokeContext::OnBindingPressed()
{
    IWorld* w = GetCommand()->GetWorld();
    CameraMap* cm = w->GetCameraMap();
    if(!cm->empty())
    {
        w->CurrentCamera(&cm->begin()->second);
    }
}




CurrentCameraFirstCommand::CurrentCameraFirstCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr CurrentCameraFirstCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new CurrentCameraFirstCommandInvokeContext(shared_from_this()));
}
