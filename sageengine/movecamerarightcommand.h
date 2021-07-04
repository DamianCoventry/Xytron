#ifndef INCLUDED_MOVECAMERARIGHTCOMMAND
#define INCLUDED_MOVECAMERARIGHTCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class MoveCameraRightCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    MoveCameraRightCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingHeld();
};

class MoveCameraRightCommand
    : public Command
{
public:
    MoveCameraRightCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_MOVECAMERARIGHTCOMMAND
