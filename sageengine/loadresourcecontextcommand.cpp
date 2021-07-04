#include "loadresourcecontextcommand.h"
#include "iworld.h"
#include "../aud/device.h"
#include "../sagedocuments/gamedocument.h"

void LoadResourceContextCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    // Get a pointer to the resource context data from the game document.
    MyTest::ResourceContextDocumentMap& rcdm = game_doc_->ResourceContextDocuments();
    MyTest::ResourceContextDocumentMap::iterator rcdm_itor = rcdm.find(res_cxt_name_);
    if(rcdm_itor == rcdm.end())
    {
        throw std::runtime_error(std::string("Can't load the resource context named [") + res_cxt_name_+ "] because it doesn't exist within the game");
    }
    MyTest::ResourceContextDocument* rcd = &rcdm_itor->second;

    // Create a new resource context in the world.
    Util::ResourceContextMap* res_cxt_map = world->GetResourceContextMap();
    res_cxt_map->insert(std::make_pair(res_cxt_name_, Util::ResourceContext(audio_device_)));

    Util::ResourceContextMap::iterator res_cxt_itor = res_cxt_map->find(res_cxt_name_);
    Util::ResourceContext* res_cxt = &res_cxt_itor->second;

    // Iterate over the resource context data from the game document and load it all into
    // the world.
    MyTest::ResourceContextDocument::iterator rcd_itor;
    for(rcd_itor = rcd->begin(); rcd_itor != rcd->end(); ++rcd_itor)
    {
        switch(rcd_itor->second.type_)
        {
        case MyTest::RT_IMAGE:
            {
                // Lookup this image in the game document
                ImageDocumentMap& idm = game_doc_->ImageDocuments();
                ImageDocumentMap::iterator idm_itor = idm.find(rcd_itor->first);
                if(idm_itor == idm.end())
                {
                    throw std::runtime_error(std::string("Can't load the image named [") + rcd_itor->first+ "] because it doesn't exist within the game");
                }
                res_cxt->LoadImage(
                    idm_itor->second.TextureFilename(),
                    idm_itor->second.FrameX(),
                    idm_itor->second.FrameY(),
                    idm_itor->second.FrameWidth(),
                    idm_itor->second.FrameHeight());
                break;
            }
        case MyTest::RT_IMAGE_ANIMATION:
            {
                // Lookup this image animation in the game document
                ImageAnimationDocumentMap& iadm = game_doc_->ImageAnimationDocuments();
                ImageAnimationDocumentMap::iterator iadm_itor = iadm.find(rcd_itor->first);
                if(iadm_itor == iadm.end())
                {
                    throw std::runtime_error(std::string("Can't load the image animation named [") + rcd_itor->first+ "] because it doesn't exist within the game");
                }
                res_cxt->LoadImageAnim(
                    iadm_itor->second.TextureFilename(),
                    iadm_itor->second.StartX(),
                    iadm_itor->second.StartY(),
                    iadm_itor->second.FrameWidth(),
                    iadm_itor->second.FrameHeight(),
                    iadm_itor->second.FrameCount());
                break;
            }
        case MyTest::RT_SOUND:
            res_cxt->LoadSound(rcd_itor->first, 1.0f, 5.0f);        // TODO: Load the 1.0f and 5.0f from the game document
            break;
        case MyTest::RT_MUSIC:
            res_cxt->LoadMusic(rcd_itor->first, window_, window_message_id_);
            break;
        case MyTest::RT_TEXTURE:
            res_cxt->LoadTexture(world->GetContentDir() + "\\Textures\\" + rcd_itor->first);
            break;
        }

    }
}




LoadResourceContextCommand::LoadResourceContextCommand(IWorld* world, GameDocument* game_doc, Aud::Device* audio_device, HWND window, int window_message_id)
: Command(world)
, game_doc_(game_doc)
, audio_device_(audio_device)
, window_(window)
, window_message_id_(window_message_id)
{
}

CommandInvokeContextPtr LoadResourceContextCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the resource context to load

    if(command_line.size() < 2)
    {
        throw std::runtime_error("Insufficient number of command line arguments for LoadResourceContextCommand");
    }

    return CommandInvokeContextPtr(new LoadResourceContextCommandInvokeContext(shared_from_this(), game_doc_, command_line[1], audio_device_, window_, window_message_id_));
}
