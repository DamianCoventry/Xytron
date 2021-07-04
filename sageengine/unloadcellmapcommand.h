#ifndef INCLUDED_UNLOADCELLMAPCOMMAND
#define INCLUDED_UNLOADCELLMAPCOMMAND

#include "command.h"

class UnloadCellMapCommandInvokeContext
    : public CommandInvokeContext
{
public:
    UnloadCellMapCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    void Execute();

private:
};

class UnloadCellMapCommand
    : public Command
{
public:
    UnloadCellMapCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_UNLOADCELLMAPCOMMAND
