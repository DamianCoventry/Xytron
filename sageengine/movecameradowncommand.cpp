#include "movecameradowncommand.h"
#include "iworld.h"
#include "../sagedocuments/cellmapdocument.h"
#include "../math/linesegment.h"

void MoveCameraDownCommandInvokeContext::OnBindingHeld()
{
    IWorld* w = GetCommand()->GetWorld();
    Camera* c = w->CurrentCamera();
    if(c)
    {
        if(w->CameraCollision())
        {
            Math::Vector begin(c->Position());
            Math::Vector end(c->GetMoveDown(w->TimeDelta()));

            //if(w->GetCellMap()->IsLineOfSight(Math::LineSegment(begin, end), 20.0f))
            //{
            //    c->Position(end);
            //}
            end = w->GetCellMap()->TraceLineSegment(Math::LineSegment(begin, end), 20.0f);
            c->Position(end);
        }
        else
        {
            c->MoveDown(w->TimeDelta());
        }
    }
}




MoveCameraDownCommand::MoveCameraDownCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr MoveCameraDownCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new MoveCameraDownCommandInvokeContext(shared_from_this()));
}
