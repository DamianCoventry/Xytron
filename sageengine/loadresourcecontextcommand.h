#ifndef INCLUDED_LOADRESOURCECONTEXTCOMMAND
#define INCLUDED_LOADRESOURCECONTEXTCOMMAND

#include "command.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class GameDocument;
namespace Aud
{
    class Device;
}

class LoadResourceContextCommandInvokeContext
    : public CommandInvokeContext
{
public:
    LoadResourceContextCommandInvokeContext(CommandPtr command, GameDocument* game_doc, const std::string& res_cxt_name, Aud::Device* audio_device, HWND window, int window_message_id)
        : CommandInvokeContext(command), game_doc_(game_doc), res_cxt_name_(res_cxt_name), audio_device_(audio_device), window_(window), window_message_id_(window_message_id) {}

    void Execute();

private:
    HWND window_;
    int window_message_id_;
    Aud::Device* audio_device_;
    GameDocument* game_doc_;
    std::string res_cxt_name_;
};

class LoadResourceContextCommand
    : public Command
{
public:
    LoadResourceContextCommand(IWorld* world, GameDocument* game_doc, Aud::Device* audio_device, HWND window, int window_message_id);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);

private:
    GameDocument* game_doc_;
    Aud::Device* audio_device_;
    HWND window_;
    int window_message_id_;
};

#endif  // INCLUDED_LOADRESOURCECONTEXTCOMMAND
