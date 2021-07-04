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
#include "device.h"
#include "sound.h"

#include "../util/guidutil.h"
#include "../util/throw.h"

#include <sstream>
#include <stdexcept>

using namespace Aud;

///////////////////////////////////////////////////////////////////////////////
Device::Device()
: dsound_(NULL)
, primary_(NULL)
, listener_(NULL)
, enabled_(true)
{
}

///////////////////////////////////////////////////////////////////////////////
Device::~Device()
{
    Shutdown();
}

///////////////////////////////////////////////////////////////////////////////
void Device::Initialise(HWND window, GUID* guid, int sps, int bps, int channels)
{
    if(guid)
    {
        std::string guid_str;
        Util::GuidToString(*guid, &guid_str);
        LOG("Initialising audio device [" << guid_str << "]");
    }
    else
    {
        LOG("Initialising the default audio device");
    }

    HRESULT hr = CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC, IID_IDirectSound8,
        reinterpret_cast<void**>(&dsound_));
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of CLSID_DirectSound8")
    }

    hr = dsound_->Initialize(guid);
    if(FAILED(hr))
    {
        THROW_COM("Could not initialise an instance of CLSID_DirectSound8")
    }

    hr = dsound_->SetCooperativeLevel(window, DSSCL_PRIORITY);
    if(FAILED(hr))
    {
        THROW_COM("Could not initialise an instance of CLSID_DirectSound8")
    }

    CreatePrimaryBuffer();
    SetPrimaryBufferFormat(sps, bps, channels);
    Create3dListener();
}

///////////////////////////////////////////////////////////////////////////////
void Device::Shutdown()
{
    if(listener_)
    {
        listener_->Release();
        listener_ = NULL;
    }
    if(primary_)
    {
        primary_->Release();
        primary_ = NULL;
    }
    if(dsound_)
    {
        LOG("Shutting the audio device down");
        dsound_->Release();
        dsound_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
SoundPtr Device::CreateSound(boost::shared_array<unsigned char> bytes, int num_bytes, float min_distance, float max_distance)
{
    SoundPtr sound(new Sound(this));
    sound->Create(dsound_, bytes, num_bytes, min_distance, max_distance);
    return sound;
}

///////////////////////////////////////////////////////////////////////////////
void Device::CreatePrimaryBuffer()
{
    LOG("Initialising the audio device's primary buffer");

    DSBUFFERDESC info;
    memset(&info, 0, sizeof(DSBUFFERDESC));

    info.dwSize         = sizeof(DSBUFFERDESC);
    info.dwFlags        = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

    HRESULT hr = dsound_->CreateSoundBuffer(&info, &primary_, NULL);
    if(FAILED(hr))
    {
        throw std::runtime_error("Could not create the primary sound buffer");
    }

    primary_->Play(0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
void Device::SetPrimaryBufferFormat(int sps, int bps, int channels)
{
    LOG("Setting the audio format to [" << sps << ", " << bps << ", " << channels << "]");

    memset(&format_, 0, sizeof(WAVEFORMATEX));

    format_.wFormatTag       = WAVE_FORMAT_PCM;
    format_.nChannels        = channels;
    format_.nSamplesPerSec   = sps;
    format_.wBitsPerSample   = bps;
    format_.nBlockAlign      = (format_.nChannels * format_.wBitsPerSample) >> 3;
    format_.nAvgBytesPerSec  = format_.nSamplesPerSec * format_.nBlockAlign;

    HRESULT hr = primary_->SetFormat(&format_);
    if(FAILED(hr))
    {
        THROW_COM("Could not set the primary sound buffer's format")
    }
}

///////////////////////////////////////////////////////////////////////////////
void Device::Create3dListener()
{
    DS3DLISTENER params;

    LOG("Creating the 3D listener");

    HRESULT hr = primary_->QueryInterface(IID_IDirectSound3DListener, (void**)&listener_);
    if(FAILED(hr))
    {
        THROW_COM("Couldn't create the 3D sound listener")
    }

    params.dwSize = sizeof(DS3DLISTENER);
    listener_->GetAllParameters(&params);

    params.flDopplerFactor = DS3D_DEFAULTDOPPLERFACTOR;
    params.flRolloffFactor = DS3D_DEFAULTROLLOFFFACTOR;
    listener_->SetAllParameters(&params, DS3D_IMMEDIATE);
}

///////////////////////////////////////////////////////////////////////////////
void Device::SetListenerPosition(const Math::Vector& position,
                                 const Math::Vector& forward,
                                 const Math::Vector& up)
{
    DS3DLISTENER params;

    if(listener_)
    {
        params.dwSize = sizeof(DS3DLISTENER);
        listener_->GetAllParameters(&params);

        params.vPosition.x = position.x_;
        params.vPosition.y = position.y_;
        params.vPosition.z = -position.z_;        // flip it coz ogl is right handed

        params.vOrientFront.x = forward.x_;
        params.vOrientFront.y = forward.y_;
        params.vOrientFront.z = -forward.z_;

        params.vOrientTop.x = up.x_;
        params.vOrientTop.y = up.y_;
        params.vOrientTop.z = -up.z_;

        listener_->SetAllParameters(&params, DS3D_IMMEDIATE);
    }
}
