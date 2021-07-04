#ifndef INCLUDED_DRAWCELLMAPCOMMAND
#define INCLUDED_DRAWCELLMAPCOMMAND

#include "command.h"
#include "../gfx/texture.h"

class CellMapDocument;

class DrawCellMapCommandInvokeContext
    : public CommandInvokeContext
{
public:
    DrawCellMapCommandInvokeContext(CommandPtr command);

    void Execute();

private:
    CellMapDocument* map_doc_;
};

class DrawCellMapCommand
    : public Command
{
public:
    DrawCellMapCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_DRAWCELLMAPCOMMAND
