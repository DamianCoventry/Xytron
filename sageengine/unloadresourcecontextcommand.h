#ifndef INCLUDED_UNLOADRESOURCECONTEXTCOMMAND
#define INCLUDED_UNLOADRESOURCECONTEXTCOMMAND

#include "command.h"
#include "../sagedocuments/resourcecontextdocument.h"

class UnloadResourceContextCommandInvokeContext
    : public CommandInvokeContext
{
public:
    UnloadResourceContextCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name) {}

    void Execute();

private:
    std::string res_cxt_name_;
};

class UnloadResourceContextCommand
    : public Command
{
public:
    UnloadResourceContextCommand(IWorld* world, MyTest::ResourceContextDocumentMap* res_cxt_map);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);

private:
    MyTest::ResourceContextDocumentMap* res_cxt_map_;
};

#endif  // INCLUDED_UNLOADRESOURCECONTEXTCOMMAND
