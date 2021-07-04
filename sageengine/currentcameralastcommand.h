#ifndef INCLUDED_CURRENTCAMERALASTCOMMAND
#define INCLUDED_CURRENTCAMERALASTCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class CurrentCameraLastCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    CurrentCameraLastCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingPressed();
};

class CurrentCameraLastCommand
    : public Command
{
public:
    CurrentCameraLastCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_CURRENTCAMERALASTCOMMAND
