#include "drawcellmapcommand.h"
#include "iworld.h"
#include "../sagedocuments/cellmapdocument.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>

DrawCellMapCommandInvokeContext::DrawCellMapCommandInvokeContext(CommandPtr command)
: CommandInvokeContext(command)
{
    map_doc_ = command->GetWorld()->GetCellMap();
}

void DrawCellMapCommandInvokeContext::Execute()
{
    map_doc_->DrawCellsOpaque_Textured();
    map_doc_->DrawSky();
    map_doc_->DrawCellsTranslucent_Textured();
}




DrawCellMapCommand::DrawCellMapCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr DrawCellMapCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new DrawCellMapCommandInvokeContext(shared_from_this()));
}
