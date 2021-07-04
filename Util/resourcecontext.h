///////////////////////////////////////////////////////////////////////////////
//
//  PROJECT: Sage
//
//  AUTHOR: Damian Coventry
//
//  CREATION DATE: June-July 2007
// 
//  COPYRIGHT NOTICE:
//
//      (C) Omenware
//      Created in 2007 as an unpublished copyright work.  All rights reserved.
//      This document and the information it contains is confidential and
//      proprietary to Omenware.  Hence, it may not be  used, copied, reproduced,
//      transmitted, or stored in any form or by any means, electronic,
//      recording, photocopying, mechanical or otherwise, without the prior
//      written permission of Omenware
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#ifndef INCLUDED_UTIL_RESOURCECONTEXT
#define INCLUDED_UTIL_RESOURCECONTEXT

#include "../gfx/texture.h"
#include "../gfx/font.h"
#include "../gfx/image.h"
#include "../gfx/imageanim.h"
#include "../aud/sound.h"
#include "../aud/music.h"

#undef LoadImage

///////////////////////////////////////////////////////////////////////////////
namespace Util
{

///////////////////////////////////////////////////////////////////////////////
class ResourceContext
{
public:
    ResourceContext(Aud::Device* audio_device)
        : audio_device_(audio_device) {}
    ~ResourceContext() { UnloadAll(); }

    void CreateDefaultTexture();
    static const std::string& GetDefaultTextureName();
    Gfx::TexturePtr GetDefaultTexture() const { return default_texture_; }

    Gfx::TexturePtr LoadTexture(const std::string& filename, bool trans = false, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
    Gfx::FontPtr LoadFont(const std::string& filename);
    Gfx::ImagePtr LoadImage(const std::string& filename, int start_x, int start_y, int frame_width, int frame_height, bool trans = false, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
    Gfx::ImageAnimPtr LoadImageAnim(const std::string& filename, int start_x, int start_y, int frame_width, int frame_height, int frame_count, bool trans = false, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
    Aud::SoundPtr LoadSound(const std::string& filename, float min_distance, float max_distance);
    Aud::MusicPtr LoadMusic(const std::string& filename, HWND window, int window_message_id);

    Gfx::TexturePtr UnloadTexture(const std::string& filename);
    Gfx::FontPtr UnloadFont(const std::string& filename);
    Gfx::ImagePtr UnloadImage(const std::string& filename);
    Gfx::ImageAnimPtr UnloadImageAnim(const std::string& filename);
    Aud::SoundPtr UnloadSound(const std::string& filename);
    Aud::MusicPtr UnloadMusic(const std::string& filename);

    Gfx::TexturePtr FindTexture(const std::string& filename);
    Gfx::FontPtr FindFont(const std::string& filename);
    Gfx::ImagePtr FindImage(const std::string& filename);
    Gfx::ImageAnimPtr FindImageAnim(const std::string& filename);
    Aud::SoundPtr FindSound(const std::string& filename);
    Aud::MusicPtr FindMusic(const std::string& filename);

    void UnloadTexures()    { texture_list_.clear(); }
    void UnloadFonts()      { font_list_.clear(); }
    void UnloadImages()     { image_list_.clear(); }
    void UnloadImageLists() { imageanim_list_.clear(); }
    void UnloadSounds()     { sound_list_.clear(); }
    void UnloadMusics()     { music_list_.clear(); }
    void UnloadAll()        { UnloadTexures(); UnloadFonts(); UnloadImages(); UnloadImageLists(); UnloadSounds(); }

    void ApplySoundVolume(unsigned long volume);

private:
    static const std::string default_texture_name_;
    Gfx::TexturePtr     default_texture_;
    Gfx::TextureList    texture_list_;
    Gfx::FontList       font_list_;
    Gfx::ImageList      image_list_;
    Gfx::ImageAnimList  imageanim_list_;
    Aud::SoundList      sound_list_;
    Aud::Device*        audio_device_;
    Aud::MusicList      music_list_;
};

typedef std::map<std::string, ResourceContext> ResourceContextMap;

}       // namespace Util

#endif  // INCLUDED_UTIL_RESOURCECONTEXT
