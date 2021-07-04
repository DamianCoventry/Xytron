#include "activatebindingscommand.h"
#include "iworld.h"
#include "../Inp/bindings.h"

void ActivateBindingsCommandInvokeContext::Execute()
{
    bindings_->Activate();
}




ActivateBindingsCommand::ActivateBindingsCommand(IWorld* world, Inp::Bindings* bindings)
: Command(world)
, bindings_(bindings)
{
}

CommandInvokeContextPtr ActivateBindingsCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command

    return CommandInvokeContextPtr(new ActivateBindingsCommandInvokeContext(shared_from_this(), bindings_));
}
