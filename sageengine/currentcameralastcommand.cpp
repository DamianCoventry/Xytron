#include "currentcameralastcommand.h"
#include "iworld.h"

void CurrentCameraLastCommandInvokeContext::OnBindingPressed()
{
    IWorld* w = GetCommand()->GetWorld();
    CameraMap* cm = w->GetCameraMap();
    if(!cm->empty())
    {
        CameraMap::iterator i = cm->end();
        --i;
        w->CurrentCamera(&i->second);
    }
}




CurrentCameraLastCommand::CurrentCameraLastCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr CurrentCameraLastCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new CurrentCameraLastCommandInvokeContext(shared_from_this()));
}
