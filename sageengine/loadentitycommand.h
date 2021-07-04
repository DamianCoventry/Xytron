#ifndef INCLUDED_LOADENTITYCOMMAND
#define INCLUDED_LOADENTITYCOMMAND

#include "command.h"

class LoadEntityCommandInvokeContext
    : public CommandInvokeContext
{
public:
    LoadEntityCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& name)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), name_(name) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string name_;
};

class LoadEntityCommand
    : public Command
{
public:
    LoadEntityCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_LOADENTITYCOMMAND
