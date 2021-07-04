#ifndef INCLUDED_MOVECAMERAUPCOMMAND
#define INCLUDED_MOVECAMERAUPCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class MoveCameraUpCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    MoveCameraUpCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingHeld();
};

class MoveCameraUpCommand
    : public Command
{
public:
    MoveCameraUpCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_MOVECAMERAUPCOMMAND
