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
#include "stdafx.h"
#include "resourcecontext.h"
#include "../util/throw.h"

using namespace Util;

///////////////////////////////////////////////////////////////////////////////
const std::string ResourceContext::default_texture_name_ = "(DefaultTexture)";

///////////////////////////////////////////////////////////////////////////////
void ResourceContext::CreateDefaultTexture()
{
    default_texture_ = Gfx::Texture::CreateDefaultTexture();
    if(default_texture_)
    {
        texture_list_[default_texture_name_] = default_texture_;
    }
}

///////////////////////////////////////////////////////////////////////////////
Gfx::TexturePtr ResourceContext::LoadTexture(const std::string& filename, bool trans, unsigned char r, unsigned char g, unsigned char b)
{
    LOG("ResourceContext checking texture [" << filename << "]");
    Gfx::TextureList::iterator itor = texture_list_.find(filename);
    if(itor != texture_list_.end())
    {
        LOG("Texture [" << filename << "] is already loaded, returning shared_ptr to cached copy");
        return itor->second;
    }
    LOG("Texture [" << filename << "] is absent, attemping to load from disk");
    Gfx::TexturePtr texture = Gfx::Texture::CreateFromFile(filename, trans, r, g, b);
    if(texture)
    {
        LOG("Caching a shared_ptr to texture [" << filename << "] in the ResourceContext");
        texture_list_[filename] = texture;
    }
    return texture;
}

///////////////////////////////////////////////////////////////////////////////
Gfx::FontPtr ResourceContext::LoadFont(const std::string& filename)
{
    LOG("ResourceContext checking font [" << filename << "]");
    Gfx::FontList::iterator itor = font_list_.find(filename);
    if(itor != font_list_.end())
    {
        LOG("Font [" << filename << "] is already loaded, returning shared_ptr to cached copy");
        return itor->second;
    }
    LOG("Font [" << filename << "] is absent, attemping to load from disk");
    Gfx::FontPtr font = Gfx::Font::CreateFromFile(filename);
    if(font)
    {
        LOG("Caching a shared_ptr to font [" << filename << "] in the ResourceContext");
        font_list_[filename] = font;
    }
    return font;
}

///////////////////////////////////////////////////////////////////////////////
Gfx::ImagePtr ResourceContext::LoadImage(const std::string& filename, int start_x, int start_y, int frame_width, int frame_height, bool trans, unsigned char r, unsigned char g, unsigned char b)
{
    LOG("ResourceContext checking image [" << filename << "]");
    Gfx::ImageList::iterator itor = image_list_.find(filename);
    if(itor != image_list_.end())
    {
        LOG("Image [" << filename << "] is already loaded, returning shared_ptr to cached copy");
        return itor->second;
    }
    LOG("Image [" << filename << "] is absent, attemping to load from disk");
    Gfx::TexturePtr tex = LoadTexture(filename, trans, r, g, b);
    Gfx::ImagePtr image = Gfx::Image::CreateFromTexture(tex, start_x, start_y, frame_width, frame_height);
    if(image)
    {
        LOG("Caching a shared_ptr to image [" << filename << "] in the ResourceContext");
        image_list_[filename] = image;
    }
    return image;
}

///////////////////////////////////////////////////////////////////////////////
Gfx::ImageAnimPtr ResourceContext::LoadImageAnim(const std::string& filename, int start_x, int start_y, int frame_width, int frame_height, int frame_count, bool trans, unsigned char r, unsigned char g, unsigned char b)
{
    LOG("ResourceContext checking image animation [" << filename << "]");
    Gfx::ImageAnimList::iterator itor = imageanim_list_.find(filename);
    if(itor != imageanim_list_.end())
    {
        LOG("Image animation [" << filename << "] is already loaded, returning shared_ptr to cached copy");
        return itor->second;
    }
    LOG("Image animation [" << filename << "] is absent, attemping to load from disk");
    Gfx::TexturePtr tex = LoadTexture(filename, trans, r, g, b);
    Gfx::ImageAnimPtr imagelist = Gfx::ImageAnim::CreateFromTexture(tex, start_x, start_y, frame_width, frame_height, frame_count);
    if(imagelist)
    {
        LOG("Caching a shared_ptr to image animation [" << filename << "] in the ResourceContext");
        imageanim_list_[filename] = imagelist;
    }
    return imagelist;
}

///////////////////////////////////////////////////////////////////////////////
Aud::SoundPtr ResourceContext::LoadSound(const std::string& filename, float min_distance, float max_distance)
{
    LOG("ResourceContext checking sound [" << filename << "]");
    Aud::SoundList::iterator itor = sound_list_.find(filename);
    if(itor != sound_list_.end())
    {
        LOG("Sound [" << filename << "] is already loaded, returning shared_ptr to cached copy");
        return itor->second;
    }
    LOG("Sound [" << filename << "] is absent, attemping to load from disk");
    Aud::SoundPtr sound = Aud::Sound::CreateFromFile(filename, audio_device_, min_distance, max_distance);
    if(sound)
    {
        LOG("Caching a shared_ptr to sound [" << filename << "] in the ResourceContext");
        sound_list_[filename] = sound;
    }
    return sound;
}

///////////////////////////////////////////////////////////////////////////////
Aud::MusicPtr ResourceContext::LoadMusic(const std::string& filename, HWND window, int window_message_id)
{
    LOG("ResourceContext checking music [" << filename << "]");
    Aud::MusicList::iterator itor = music_list_.find(filename);
    if(itor != music_list_.end())
    {
        LOG("Music [" << filename << "] is already loaded, returning shared_ptr to cached copy");
        return itor->second;
    }
    LOG("Music [" << filename << "] is absent, attemping to load from disk");
    Aud::MusicPtr music(new Aud::Music);
    if(music)
    {
        music->Load(filename, window, window_message_id);

        LOG("Caching a shared_ptr to music [" << filename << "] in the ResourceContext");
        music_list_[filename] = music;
    }
    return music;
}





///////////////////////////////////////////////////////////////////////////////
Gfx::TexturePtr ResourceContext::UnloadTexture(const std::string& filename)
{
    LOG("ResourceContext unloading texture [" << filename << "]");
    Gfx::TextureList::iterator itor = texture_list_.find(filename);
    if(itor != texture_list_.end())
    {
        Gfx::TexturePtr texture = itor->second;
        texture_list_.erase(itor);
        return texture;
    }
    LOG("Warning - Asked to unload texture [" << filename << "], but it was never loaded");
    return Gfx::TexturePtr();
}

///////////////////////////////////////////////////////////////////////////////
Gfx::FontPtr ResourceContext::UnloadFont(const std::string& filename)
{
    LOG("ResourceContext unloading font [" << filename << "]");
    Gfx::FontList::iterator itor = font_list_.find(filename);
    if(itor != font_list_.end())
    {
        Gfx::FontPtr font = itor->second;
        font_list_.erase(itor);
        return font;
    }
    LOG("Warning - Asked to unload font [" << filename << "], but it was never loaded");
    return Gfx::FontPtr();
}

///////////////////////////////////////////////////////////////////////////////
Gfx::ImagePtr ResourceContext::UnloadImage(const std::string& filename)
{
    LOG("ResourceContext unloading image [" << filename << "]");
    Gfx::ImageList::iterator itor = image_list_.find(filename);
    if(itor != image_list_.end())
    {
        Gfx::ImagePtr image = itor->second;
        image_list_.erase(itor);
        return image;
    }
    LOG("Warning - Asked to unload image [" << filename << "], but it was never loaded");
    return Gfx::ImagePtr();
}

///////////////////////////////////////////////////////////////////////////////
Gfx::ImageAnimPtr ResourceContext::UnloadImageAnim(const std::string& filename)
{
    LOG("ResourceContext unloading image animation [" << filename << "]");
    Gfx::ImageAnimList::iterator itor = imageanim_list_.find(filename);
    if(itor != imageanim_list_.end())
    {
        Gfx::ImageAnimPtr imagelist = itor->second;
        imageanim_list_.erase(itor);
        return imagelist;
    }
    LOG("Warning - Asked to unload image animation [" << filename << "], but it was never loaded");
    return Gfx::ImageAnimPtr();
}

///////////////////////////////////////////////////////////////////////////////
Aud::SoundPtr ResourceContext::UnloadSound(const std::string& filename)
{
    LOG("ResourceContext unloading sound [" << filename << "]");
    Aud::SoundList::iterator itor = sound_list_.find(filename);
    if(itor != sound_list_.end())
    {
        Aud::SoundPtr sound = itor->second;
        sound_list_.erase(itor);
        return sound;
    }
    LOG("Warning - Asked to unload sound [" << filename << "], but it was never loaded");
    return Aud::SoundPtr();
}

///////////////////////////////////////////////////////////////////////////////
Aud::MusicPtr ResourceContext::UnloadMusic(const std::string& filename)
{
    LOG("ResourceContext unloading music [" << filename << "]");
    Aud::MusicList::iterator itor = music_list_.find(filename);
    if(itor != music_list_.end())
    {
        Aud::MusicPtr music = itor->second;
        music_list_.erase(itor);
        return music;
    }
    LOG("Warning - Asked to unload music [" << filename << "], but it was never loaded");
    return Aud::MusicPtr();
}





///////////////////////////////////////////////////////////////////////////////
Gfx::TexturePtr ResourceContext::FindTexture(const std::string& filename)
{
    Gfx::TextureList::iterator itor = texture_list_.find(filename);
    if(itor == texture_list_.end())
    {
        THROW("The ResourceContext couldn't find texture [" << filename << "]");
    }
    return itor->second;
}

///////////////////////////////////////////////////////////////////////////////
Gfx::FontPtr ResourceContext::FindFont(const std::string& filename)
{
    Gfx::FontList::iterator itor = font_list_.find(filename);
    if(itor == font_list_.end())
    {
        THROW("The ResourceContext couldn't find font [" << filename << "]");
    }
    return itor->second;
}

///////////////////////////////////////////////////////////////////////////////
Gfx::ImagePtr ResourceContext::FindImage(const std::string& filename)
{
    Gfx::ImageList::iterator itor = image_list_.find(filename);
    if(itor == image_list_.end())
    {
        THROW("The ResourceContext couldn't find image [" << filename << "]");
    }
    return itor->second;
}

///////////////////////////////////////////////////////////////////////////////
Gfx::ImageAnimPtr ResourceContext::FindImageAnim(const std::string& filename)
{
    Gfx::ImageAnimList::iterator itor = imageanim_list_.find(filename);
    if(itor == imageanim_list_.end())
    {
        THROW("The ResourceContext couldn't find image animation [" << filename << "]");
    }
    return itor->second;
}

///////////////////////////////////////////////////////////////////////////////
Aud::SoundPtr ResourceContext::FindSound(const std::string& filename)
{
    Aud::SoundList::iterator itor = sound_list_.find(filename);
    if(itor == sound_list_.end())
    {
        THROW("The ResourceContext couldn't find sound [" << filename << "]");
    }
    return itor->second;
}

///////////////////////////////////////////////////////////////////////////////
void ResourceContext::ApplySoundVolume(unsigned long volume)
{
    Aud::SoundList::iterator itor;
    for(itor = sound_list_.begin(); itor != sound_list_.end(); ++itor)
    {
        itor->second->SetVolume(volume);
    }
}

///////////////////////////////////////////////////////////////////////////////
Aud::MusicPtr ResourceContext::FindMusic(const std::string& filename)
{
    Aud::MusicList::iterator itor = music_list_.find(filename);
    if(itor == music_list_.end())
    {
        THROW("The ResourceContext couldn't find music [" << filename << "]");
    }
    return itor->second;
}
