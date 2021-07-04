#include "unloadcellmapcommand.h"
#include "../sagedocuments/cellmapdocument.h"
#include "iworld.h"

void UnloadCellMapCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();
    world->GetCellMap()->ClearAllCells();
}




UnloadCellMapCommand::UnloadCellMapCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr UnloadCellMapCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new UnloadCellMapCommandInvokeContext(shared_from_this()));
}
