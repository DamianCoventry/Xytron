#include "currentcameraprevcommand.h"
#include "iworld.h"

void CurrentCameraPrevCommandInvokeContext::OnBindingPressed()
{
    IWorld* w = GetCommand()->GetWorld();
    CameraMap* cm = w->GetCameraMap();
    CameraMap::iterator i;
    CameraMap::iterator p;
    for(i = cm->begin(); i != cm->end(); ++i)
    {
        if(w->CurrentCamera() == &i->second)
        {
            if(i == cm->begin())
            {
                CameraMap::iterator t = cm->end();
                --t;
                w->CurrentCamera(&t->second);
            }
            else
            {
                w->CurrentCamera(&p->second);
            }
            break;
        }
        p = i;
    }
}




CurrentCameraPrevCommand::CurrentCameraPrevCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr CurrentCameraPrevCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new CurrentCameraPrevCommandInvokeContext(shared_from_this()));
}
