#ifndef INCLUDED_CURRENTCAMERASETCOMMAND
#define INCLUDED_CURRENTCAMERASETCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class CurrentCameraSetCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    CurrentCameraSetCommandInvokeContext(CommandPtr command, const std::string& name)
        : CommandInvokeContext(command), name_(name) {}

    void Execute();

    // BindingEvents
    void OnBindingPressed();

private:
    std::string name_;
};

class CurrentCameraSetCommand
    : public Command
{
public:
    CurrentCameraSetCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_CURRENTCAMERASETCOMMAND
