#ifndef INCLUDED_MOVECAMERALEFTCOMMAND
#define INCLUDED_MOVECAMERALEFTCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class MoveCameraLeftCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    MoveCameraLeftCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingHeld();
};

class MoveCameraLeftCommand
    : public Command
{
public:
    MoveCameraLeftCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_MOVECAMERALEFTCOMMAND
