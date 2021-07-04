#include "movecamerabackwardcommand.h"
#include "iworld.h"
#include "../sagedocuments/cellmapdocument.h"
#include "../math/linesegment.h"

void MoveCameraBackwardCommandInvokeContext::OnBindingHeld()
{
    IWorld* w = GetCommand()->GetWorld();
    Camera* c = w->CurrentCamera();
    if(c)
    {
        if(w->CameraCollision())
        {
            Math::Vector begin(c->Position());
            Math::Vector end(c->GetMoveBackward(w->TimeDelta()));

            //if(w->GetCellMap()->IsLineOfSight(Math::LineSegment(begin, end), 20.0f))
            //{
            //    c->Position(end);
            //}
            end = w->GetCellMap()->TraceLineSegment(Math::LineSegment(begin, end), 20.0f);
            c->Position(end);
        }
        else
        {
            c->MoveBackward(w->TimeDelta());
        }
    }
}




MoveCameraBackwardCommand::MoveCameraBackwardCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr MoveCameraBackwardCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new MoveCameraBackwardCommandInvokeContext(shared_from_this()));
}
