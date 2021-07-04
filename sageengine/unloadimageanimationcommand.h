#ifndef INCLUDED_UNLOADIMAGEANIMATIONCOMMAND
#define INCLUDED_UNLOADIMAGEANIMATIONCOMMAND

#include "command.h"

class UnloadImageAnimationCommandInvokeContext
    : public CommandInvokeContext
{
public:
    UnloadImageAnimationCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& name)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), name_(name) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string name_;
};

class UnloadImageAnimationCommand
    : public Command
{
public:
    UnloadImageAnimationCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_UNLOADIMAGEANIMATIONCOMMAND
