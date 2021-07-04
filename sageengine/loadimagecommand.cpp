#include "loadimagecommand.h"
#include "iworld.h"

void LoadImageCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    Util::ResourceContext* res_cxt = world->GetResourceContext(res_cxt_name_);
    if(res_cxt == NULL)
    {
        throw std::runtime_error(std::string("Can't Load the image [") + name_ + "] from the resource context named [" + res_cxt_name_ + "], because no such resource context exists within the game");
    }

    ImageDocumentMap::iterator idm_itor = image_docs_->find(name_);
    if(idm_itor == image_docs_->end())
    {
        throw std::runtime_error(std::string("Can't load the image named [") + name_ + "] because it doesn't exist within the game");
    }

    res_cxt->LoadImage(
        idm_itor->second.TextureFilename(),
        idm_itor->second.FrameX(),
        idm_itor->second.FrameY(),
        idm_itor->second.FrameWidth(),
        idm_itor->second.FrameHeight());
}




LoadImageCommand::LoadImageCommand(IWorld* world, ImageDocumentMap* image_docs)
: Command(world)
, image_docs_(image_docs)
{
}

CommandInvokeContextPtr LoadImageCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the image to Load
    // command_line[2] is the resource context

    if(command_line.size() < 3)
    {
        throw std::runtime_error("Insufficient number of command line arguments for LoadImageCommand");
    }

    return CommandInvokeContextPtr(new LoadImageCommandInvokeContext(shared_from_this(), command_line[2], command_line[1], image_docs_));
}
