#ifndef INCLUDED_UNLOADMUSICCOMMAND
#define INCLUDED_UNLOADMUSICCOMMAND

#include "command.h"

class UnloadMusicCommandInvokeContext
    : public CommandInvokeContext
{
public:
    UnloadMusicCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& filename)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), filename_(filename) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string filename_;
};

class UnloadMusicCommand
    : public Command
{
public:
    UnloadMusicCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_UNLOADMUSICCOMMAND
