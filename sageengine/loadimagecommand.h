#ifndef INCLUDED_LOADIMAGECOMMAND
#define INCLUDED_LOADIMAGECOMMAND

#include "command.h"
#include "../sagedocuments/imagedocument.h"

class LoadImageCommandInvokeContext
    : public CommandInvokeContext
{
public:
    LoadImageCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& name, ImageDocumentMap* image_docs)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), name_(name), image_docs_(image_docs) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string name_;
    ImageDocumentMap* image_docs_;
};

class LoadImageCommand
    : public Command
{
public:
    LoadImageCommand(IWorld* world, ImageDocumentMap* image_docs);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);

private:
    ImageDocumentMap* image_docs_;
};

#endif  // INCLUDED_LOADIMAGECOMMAND
