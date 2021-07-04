#include "cameratogglecollisioncommand.h"
#include "iworld.h"

void CameraToggleCollisionCommandInvokeContext::OnBindingPressed()
{
    IWorld* w = GetCommand()->GetWorld();
    w->CameraCollision(!w->CameraCollision());
}




CameraToggleCollisionCommand::CameraToggleCollisionCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr CameraToggleCollisionCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new CameraToggleCollisionCommandInvokeContext(shared_from_this()));
}
