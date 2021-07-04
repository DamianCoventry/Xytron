#ifndef INCLUDED_MOVECAMERAFORWARDCOMMAND
#define INCLUDED_MOVECAMERAFORWARDCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class MoveCameraForwardCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    MoveCameraForwardCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingHeld();
};

class MoveCameraForwardCommand
    : public Command
{
public:
    MoveCameraForwardCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_MOVECAMERAFORWARDCOMMAND
