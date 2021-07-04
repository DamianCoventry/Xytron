#ifndef INCLUDED_LOADMUSICCOMMAND
#define INCLUDED_LOADMUSICCOMMAND

#include "command.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class LoadMusicCommandInvokeContext
    : public CommandInvokeContext
{
public:
    LoadMusicCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& filename, HWND window, int window_message_id)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), filename_(filename), window_(window), window_message_id_(window_message_id) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string filename_;
    HWND window_;
    int window_message_id_;
};

class LoadMusicCommand
    : public Command
{
public:
    LoadMusicCommand(IWorld* world, HWND window, int window_message_id);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);

private:
    HWND window_;
    int window_message_id_;
};

#endif  // INCLUDED_LOADMUSICCOMMAND
