#ifndef INCLUDED_UNLOADIMAGECOMMAND
#define INCLUDED_UNLOADIMAGECOMMAND

#include "command.h"

class UnloadImageCommandInvokeContext
    : public CommandInvokeContext
{
public:
    UnloadImageCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& name)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), name_(name) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string name_;
};

class UnloadImageCommand
    : public Command
{
public:
    UnloadImageCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_UNLOADIMAGECOMMAND
