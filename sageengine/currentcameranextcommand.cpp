#include "currentcameranextcommand.h"
#include "iworld.h"

void CurrentCameraNextCommandInvokeContext::OnBindingPressed()
{
    IWorld* w = GetCommand()->GetWorld();
    CameraMap* cm = w->GetCameraMap();
    CameraMap::iterator i;
    for(i = cm->begin(); i != cm->end(); ++i)
    {
        if(w->CurrentCamera() == &i->second)
        {
            ++i;
            if(i == cm->end())
            {
                w->CurrentCamera(&cm->begin()->second);
            }
            else
            {
                w->CurrentCamera(&i->second);
            }
            break;
        }
    }
}




CurrentCameraNextCommand::CurrentCameraNextCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr CurrentCameraNextCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new CurrentCameraNextCommandInvokeContext(shared_from_this()));
}
