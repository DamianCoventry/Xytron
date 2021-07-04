#ifndef INCLUDED_MOVECAMERABACKWARDCOMMAND
#define INCLUDED_MOVECAMERABACKWARDCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class MoveCameraBackwardCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    MoveCameraBackwardCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingHeld();
};

class MoveCameraBackwardCommand
    : public Command
{
public:
    MoveCameraBackwardCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_MOVECAMERABACKWARDCOMMAND
