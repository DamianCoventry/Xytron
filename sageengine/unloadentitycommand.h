#ifndef INCLUDED_UNLOADENTITYCOMMAND
#define INCLUDED_UNLOADENTITYCOMMAND

#include "command.h"

class UnloadEntityCommandInvokeContext
    : public CommandInvokeContext
{
public:
    UnloadEntityCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& name)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), name_(name) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string name_;
};

class UnloadEntityCommand
    : public Command
{
public:
    UnloadEntityCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_UNLOADENTITYCOMMAND
