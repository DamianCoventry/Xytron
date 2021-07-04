#ifndef INCLUDED_CAMERATOGGLECOLLISIONCOMMAND
#define INCLUDED_CAMERATOGGLECOLLISIONCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class CameraToggleCollisionCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    CameraToggleCollisionCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingPressed();
};

class CameraToggleCollisionCommand
    : public Command
{
public:
    CameraToggleCollisionCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_CAMERATOGGLECOLLISIONCOMMAND
