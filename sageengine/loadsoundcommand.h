#ifndef INCLUDED_LOADSOUNDCOMMAND
#define INCLUDED_LOADSOUNDCOMMAND

#include "command.h"

class LoadSoundCommandInvokeContext
    : public CommandInvokeContext
{
public:
    LoadSoundCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& filename)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), filename_(filename) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string filename_;
};

class LoadSoundCommand
    : public Command
{
public:
    LoadSoundCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_LOADSOUNDCOMMAND
