#ifndef INCLUDED_LOADIMAGEANIMATIONCOMMAND
#define INCLUDED_LOADIMAGEANIMATIONCOMMAND

#include "command.h"
#include "../sagedocuments/imageanimationdocument.h"

class LoadImageAnimationCommandInvokeContext
    : public CommandInvokeContext
{
public:
    LoadImageAnimationCommandInvokeContext(CommandPtr command, const std::string& res_cxt_name, const std::string& name, ImageAnimationDocumentMap* ImageAnimation_docs)
        : CommandInvokeContext(command), res_cxt_name_(res_cxt_name), name_(name), img_anim_docs_(ImageAnimation_docs) {}

    void Execute();

private:
    std::string res_cxt_name_;
    std::string name_;
    ImageAnimationDocumentMap* img_anim_docs_;
};

class LoadImageAnimationCommand
    : public Command
{
public:
    LoadImageAnimationCommand(IWorld* world, ImageAnimationDocumentMap* ImageAnimation_docs);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);

private:
    ImageAnimationDocumentMap* img_anim_docs_;
};

#endif  // INCLUDED_LOADIMAGEANIMATIONCOMMAND
