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
#include "music.h"
#include "../util/throw.h"

using namespace Aud;

///////////////////////////////////////////////////////////////////////////////
Music::Music()
: graph_builder_(NULL)
, media_control_(NULL)
, media_event_(NULL)
, media_position_(NULL)
, basic_audio_(NULL)
, enabled_(true)
{
}

///////////////////////////////////////////////////////////////////////////////
void Music::Load(const std::string& filename, HWND window, int window_message_id)
{
    LOG("Loading the music file [" << filename << "]");

    filename_ = filename;
    HRESULT hr;
    if(graph_builder_ == NULL)
    {
        hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&graph_builder_);
        if(FAILED(hr))
        {
            THROW_COM("Could not create an instance of a DirectShow FilterGraph")
        }

        // Get pointers to some useful interfaces
        graph_builder_->QueryInterface(IID_IMediaControl, (void **)&media_control_);
        graph_builder_->QueryInterface(IID_IMediaEvent, (void **)&media_event_);
        graph_builder_->QueryInterface(IID_IMediaPosition, (void **)&media_position_);
        graph_builder_->QueryInterface(IID_IBasicAudio, (void **)&basic_audio_);

        // Set our notify window
        media_event_->SetNotifyWindow((OAHWND)window, window_message_id, 0);
        media_event_->SetNotifyFlags(0);      // turn on notifications
    }

    // Load the file
    WCHAR wfilename[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, wfilename, MAX_PATH);
    wfilename[filename.size()] = 0;

    hr = graph_builder_->RenderFile(wfilename, NULL);
    if(FAILED(hr))
    {
        THROW_COM("Couldn't load the file [" << filename_ << "]")
    }

    media_position_->put_CurrentPosition(0);
}

///////////////////////////////////////////////////////////////////////////////
void Music::Unload()
{
    if(graph_builder_)
    {
        LOG("Unloading the music track [" << filename_ << "]");

        if(basic_audio_)
        {
            basic_audio_->Release();
            basic_audio_ = NULL;
        }
        if(media_position_)
        {
            media_position_->Release();
            media_position_ = NULL;
        }
        if(media_event_)
        {
            media_event_->Release();
            media_event_ = NULL;
        }
        if(media_control_)
        {
            media_control_->Release();
            media_control_ = NULL;
        }
        graph_builder_->Release();
        graph_builder_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Music::Play()
{
    if(enabled_ && media_control_)
    {
        LOG("Playing the music track [" << filename_ << "]");
        media_control_->Run();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Music::Stop()
{
    if(media_control_)
    {
        LOG("Stopping the music track [" << filename_ << "]");
        media_control_->Stop();
    }
    if(media_position_)
    {
        media_position_->put_CurrentPosition(0);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Music::Pause()
{
    if(media_control_)
    {
        LOG("Pausing the music track [" << filename_ << "]");
        media_control_->Pause();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Music::Resume()
{
    if(enabled_ && media_control_)
    {
        LOG("Resuming music");
        media_control_->Run();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Music::Volume(long volume)
{
    if(basic_audio_)
    {
        // Clamp the value
        if(volume < 0) volume = 0;
        else if(volume > 100) volume = 100;

        // Convert it to a value DirectShow expects (min=-10000, max=0)
        float percent = float(volume) / 100.0f;
        long converted_volume = -10000 + long(10000.0f * percent);

        // Apply it.
        basic_audio_->put_Volume(converted_volume);
    }
}

///////////////////////////////////////////////////////////////////////////////
long Music::Volume() const
{
    long converted_volume = 0;
    if(basic_audio_)
    {
        long volume;
        basic_audio_->get_Volume(&volume);

        // The value returned from DirectShow will be min=-10000, max=0.
        // We need to convert this value to min=0, max=100.
        converted_volume = volume + 10000;
        converted_volume = long(float(converted_volume) * 100.0f / 10000.0f);
    }
    return converted_volume;
}

///////////////////////////////////////////////////////////////////////////////
bool Music::OnGraphEvent()
{
    long event_code;
    LONG_PTR param1;
    LONG_PTR param2;
    bool track_complete_ = false;

    while(media_event_->GetEvent(&event_code, &param1, &param2, 0) != E_ABORT)
    {
        if(event_code == EC_COMPLETE)
        {
            track_complete_ = true;
        }

        media_event_->FreeEventParams(event_code, param1, param2);
    }

    return track_complete_;
}
