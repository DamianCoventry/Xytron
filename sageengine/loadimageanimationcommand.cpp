#include "loadimageanimationcommand.h"
#include "iworld.h"

void LoadImageAnimationCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    Util::ResourceContext* res_cxt = world->GetResourceContext(res_cxt_name_);
    if(res_cxt == NULL)
    {
        throw std::runtime_error(std::string("Can't Load the ImageAnimation [") + name_ + "] from the resource context named [" + res_cxt_name_ + "], because no such resource context exists within the game");
    }

    ImageAnimationDocumentMap::iterator iadm_itor = img_anim_docs_->find(name_);
    if(iadm_itor == img_anim_docs_->end())
    {
        throw std::runtime_error(std::string("Can't load the ImageAnimation named [") + name_ + "] because it doesn't exist within the game");
    }

    res_cxt->LoadImageAnim(
        iadm_itor->second.TextureFilename(),
        iadm_itor->second.StartX(),
        iadm_itor->second.StartY(),
        iadm_itor->second.FrameWidth(),
        iadm_itor->second.FrameHeight(),
        iadm_itor->second.FrameCount());
}




LoadImageAnimationCommand::LoadImageAnimationCommand(IWorld* world, ImageAnimationDocumentMap* ImageAnimation_docs)
: Command(world)
, img_anim_docs_(ImageAnimation_docs)
{
}

CommandInvokeContextPtr LoadImageAnimationCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the ImageAnimation to Load
    // command_line[2] is the resource context

    if(command_line.size() < 3)
    {
        throw std::runtime_error("Insufficient number of command line arguments for LoadImageAnimationCommand");
    }

    return CommandInvokeContextPtr(new LoadImageAnimationCommandInvokeContext(shared_from_this(), command_line[2], command_line[1], img_anim_docs_));
}
