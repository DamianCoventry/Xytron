#ifndef INCLUDED_THINKCELLMAPCOMMAND
#define INCLUDED_THINKCELLMAPCOMMAND

#include "command.h"

class ThinkCellMapCommandInvokeContext
    : public CommandInvokeContext
{
public:
    ThinkCellMapCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    void Execute(float time_delta);

private:
};

class ThinkCellMapCommand
    : public Command
{
public:
    ThinkCellMapCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_THINKCELLMAPCOMMAND
