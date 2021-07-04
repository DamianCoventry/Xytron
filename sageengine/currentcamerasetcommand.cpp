#include "currentcamerasetcommand.h"
#include "iworld.h"

void CurrentCameraSetCommandInvokeContext::Execute()
{
    IWorld* w = GetCommand()->GetWorld();
    w->CurrentCamera(name_);
}

void CurrentCameraSetCommandInvokeContext::OnBindingPressed()
{
    IWorld* w = GetCommand()->GetWorld();
    w->CurrentCamera(name_);
}




CurrentCameraSetCommand::CurrentCameraSetCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr CurrentCameraSetCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] will be the name of the camera

    if(command_line.size() < 2)
    {
        throw std::runtime_error("Insufficient parameters for CurrentCameraSetCommand");
    }

    return CommandInvokeContextPtr(new CurrentCameraSetCommandInvokeContext(shared_from_this(), command_line[1]));
}
