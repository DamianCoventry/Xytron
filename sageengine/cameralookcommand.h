#ifndef INCLUDED_CAMERALOOKCOMMAND
#define INCLUDED_CAMERALOOKCOMMAND

#include "command.h"
#include "../Inp/bindings.h"

class CameraLookCommandInvokeContext
    : public CommandInvokeContext
    , public Inp::BindingEvents
{
public:
    CameraLookCommandInvokeContext(CommandPtr command)
        : CommandInvokeContext(command) {}

    // BindingEvents
    void OnBindingMoved(int x_delta, int y_delta);
};

class CameraLookCommand
    : public Command
{
public:
    CameraLookCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_CAMERALOOKCOMMAND
