#include "thinkcellmapcommand.h"
#include "../sagedocuments/cellmapdocument.h"
#include "iworld.h"

void ThinkCellMapCommandInvokeContext::Execute(float time_delta)
{
    IWorld* world = GetCommand()->GetWorld();
    world->GetCellMap()->Think(time_delta);
}




ThinkCellMapCommand::ThinkCellMapCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr ThinkCellMapCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new ThinkCellMapCommandInvokeContext(shared_from_this()));
}
