#ifndef INCLUDED_MOVECAMERADOWNCOMMAND
#define INCLUDED_MOVECAMERADOWNCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class MoveCameraDownCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    MoveCameraDownCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingHeld();
};

class MoveCameraDownCommand
    : public Command
{
public:
    MoveCameraDownCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_MOVECAMERADOWNCOMMAND
