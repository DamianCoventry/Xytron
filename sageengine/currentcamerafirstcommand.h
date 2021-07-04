#ifndef INCLUDED_CURRENTCAMERAFIRSTCOMMAND
#define INCLUDED_CURRENTCAMERAFIRSTCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class CurrentCameraFirstCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    CurrentCameraFirstCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingPressed();
};

class CurrentCameraFirstCommand
    : public Command
{
public:
    CurrentCameraFirstCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_CURRENTCAMERAFIRSTCOMMAND
