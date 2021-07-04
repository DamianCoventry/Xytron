#ifndef INCLUDED_CURRENTCAMERANEXTCOMMAND
#define INCLUDED_CURRENTCAMERANEXTCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class CurrentCameraNextCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    CurrentCameraNextCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingPressed();
};

class CurrentCameraNextCommand
    : public Command
{
public:
    CurrentCameraNextCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_CURRENTCAMERANEXTCOMMAND
