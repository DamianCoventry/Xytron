#ifndef INCLUDED_DEACTIVATEBINDINGSCOMMAND
#define INCLUDED_DEACTIVATEBINDINGSCOMMAND

#include "command.h"

namespace Inp
{
    class Bindings;
}

class DeactivateBindingsCommandInvokeContext
    : public CommandInvokeContext
{
public:
    DeactivateBindingsCommandInvokeContext(CommandPtr command, Inp::Bindings* bindings)
        : CommandInvokeContext(command), bindings_(bindings) {}
    void Execute();
private:
    Inp::Bindings* bindings_;
};

class DeactivateBindingsCommand
    : public Command
{
public:
    DeactivateBindingsCommand(IWorld* world, Inp::Bindings* bindings);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
private:
    Inp::Bindings* bindings_;
};

#endif  // INCLUDED_DEACTIVATEBINDINGSCOMMAND
