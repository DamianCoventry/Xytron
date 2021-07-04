#include "movecamerarightcommand.h"
#include "iworld.h"
#include "../sagedocuments/cellmapdocument.h"
#include "../math/linesegment.h"

void MoveCameraRightCommandInvokeContext::OnBindingHeld()
{
    IWorld* w = GetCommand()->GetWorld();
    Camera* c = w->CurrentCamera();
    if(c)
    {
        if(w->CameraCollision())
        {
            Math::Vector begin(c->Position());
            Math::Vector end(c->GetMoveRight(w->TimeDelta()));

            //if(w->GetCellMap()->IsLineOfSight(Math::LineSegment(begin, end), 20.0f))
            //{
            //    c->Position(end);
            //}
            end = w->GetCellMap()->TraceLineSegment(Math::LineSegment(begin, end), 20.0f);
            c->Position(end);
        }
        else
        {
            c->MoveRight(w->TimeDelta());
        }
    }
}




MoveCameraRightCommand::MoveCameraRightCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr MoveCameraRightCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new MoveCameraRightCommandInvokeContext(shared_from_this()));
}
