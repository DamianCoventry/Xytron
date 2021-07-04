#include "deactivatebindingscommand.h"
#include "iworld.h"
#include "../Inp/bindings.h"

void DeactivateBindingsCommandInvokeContext::Execute()
{
    bindings_->Deactivate();
}




DeactivateBindingsCommand::DeactivateBindingsCommand(IWorld* world, Inp::Bindings* bindings)
: Command(world)
, bindings_(bindings)
{
}

CommandInvokeContextPtr DeactivateBindingsCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new DeactivateBindingsCommandInvokeContext(shared_from_this(), bindings_));
}
