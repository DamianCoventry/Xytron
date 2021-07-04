#include "cameralookcommand.h"
#include "iworld.h"

void CameraLookCommandInvokeContext::OnBindingMoved(int x_delta, int y_delta)
{
    IWorld* w = GetCommand()->GetWorld();
    Camera* c = w->CurrentCamera();
    if(c)
    {
        c->UpdateRotation(x_delta, y_delta);
    }
}




CameraLookCommand::CameraLookCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr CameraLookCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new CameraLookCommandInvokeContext(shared_from_this()));
}
