#ifndef INCLUDED_CURRENTCAMERAPREVCOMMAND
#define INCLUDED_CURRENTCAMERAPREVCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class CurrentCameraPrevCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    CurrentCameraPrevCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingPressed();
};

class CurrentCameraPrevCommand
    : public Command
{
public:
    CurrentCameraPrevCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_CURRENTCAMERAPREVCOMMAND
