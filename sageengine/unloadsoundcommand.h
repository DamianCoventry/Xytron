#ifndef INCLUDED_UNLOADSOUNDCOMMAND
#define INCLUDED_UNLOADSOUNDCOMMAND

#include "command.h"

class UnloadSoundCommandInvokeContext
    : public CommandInvokeContext
{
public:
    UnloadSoundCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& filename)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), filename_(filename) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string filename_;
};

class UnloadSoundCommand
    : public Command
{
public:
    UnloadSoundCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_UNLOADSOUNDCOMMAND
