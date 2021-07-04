#ifndef INCLUDED_ACTIVATEBINDINGSCOMMAND
#define INCLUDED_ACTIVATEBINDINGSCOMMAND

#include "command.h"

namespace Inp
{
    class Bindings;
}

class ActivateBindingsCommandInvokeContext
    : public CommandInvokeContext
{
public:
    ActivateBindingsCommandInvokeContext(CommandPtr command, Inp::Bindings* bindings)
        : CommandInvokeContext(command), bindings_(bindings) {}
    void Execute();
private:
    Inp::Bindings* bindings_;
};

class ActivateBindingsCommand
    : public Command
{
public:
    ActivateBindingsCommand(IWorld* world, Inp::Bindings* bindings);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
private:
    Inp::Bindings* bindings_;
};

#endif  // INCLUDED_ACTIVATEBINDINGSCOMMAND
