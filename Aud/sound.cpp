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
#include "sound.h"
#include "device.h"
#include "active3dsounds.h"
#include "wavefile.h"

#include <sstream>
#include <iomanip>

#include "../util/throw.h"
#include "../math/vector.h"

using namespace Aud;

///////////////////////////////////////////////////////////////////////////////
SoundPtr Sound::CreateFromFile(const std::string& filename, Device* device, float min_distance, float max_distance)
{
    Aud::WaveFile file;
    file.Load(filename);
    return device->CreateSound(file.GetBytes(), file.GetLength(), min_distance, max_distance);
}






///////////////////////////////////////////////////////////////////////////////
Sound::Sound(Device* audio_device)
: audio_device_(audio_device)
{
    for(int i = 0; i < NUM_BUFFERS; i++)
    {
        buffers_[i] = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
Sound::~Sound()
{
    Delete();
}

///////////////////////////////////////////////////////////////////////////////
void Sound::Create(IDirectSound8* dsound, boost::shared_array<unsigned char> bytes, int num_bytes, float min_distance, float max_distance)
{
    LOG("Creating a sound object from [" << num_bytes << "] bytes of audio data");

    min_distance_ = min_distance;
    max_distance_ = max_distance;

    DSBUFFERDESC info;
    memset(&info, 0, sizeof(DSBUFFERDESC));
    info.dwSize         = sizeof(DSBUFFERDESC);
    info.dwFlags        = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_LOCSOFTWARE | DSBCAPS_STATIC;
    info.dwBufferBytes  = num_bytes;
    info.lpwfxFormat    = audio_device_->GetFormat();

    IDirectSoundBuffer* buffer = NULL;
    HRESULT hr = dsound->CreateSoundBuffer(&info, &buffer, NULL);
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of a DirectSoundBuffer object");
    }

    IDirectSound3DBuffer* buffer_3d = NULL;
    hr = buffer->QueryInterface(IID_IDirectSound3DBuffer, (void**)&buffer_3d);
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of a DirectSound3DBuffer object");
    }

    current_buffer_ = 0;

    CopyAudioData(buffer, bytes, num_bytes);

    buffers_[0] = buffer;
    buffers_3d_[0] = buffer_3d;
    int i;
    for(i = 1; i < NUM_BUFFERS; i++)
    {
        hr = dsound->DuplicateSoundBuffer(buffer, &buffers_[i]);
        if(FAILED(hr))
        {
            THROW_COM("Could not duplicate a DirectSoundBuffer object");
        }
        hr = buffers_[i]->QueryInterface(IID_IDirectSound3DBuffer, (void**)&buffers_3d_[i]);
        if(FAILED(hr))
        {
            THROW_COM("Could not duplicate a DirectSound3DBuffer object");
        }
    }

    // Set the default values.
    for(i = 0; i < NUM_BUFFERS; i++)
    {
        buffers_3d_[i]->SetMinDistance(min_distance, DS3D_IMMEDIATE);
        buffers_3d_[i]->SetMaxDistance(max_distance, DS3D_IMMEDIATE);
        buffers_3d_[i]->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);
        buffers_3d_[i]->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Sound::Delete()
{
    LOG("Deleting a sound object");
    for(int i = 0; i < NUM_BUFFERS; i++)
    {
        if(buffers_[i])
        {
            buffers_[i]->Release();
            buffers_[i] = NULL;
        }
        if(buffers_3d_[i])
        {
            buffers_3d_[i]->Release();
            buffers_3d_[i] = NULL;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Sound::Play2d(bool looped)
{
    if(!audio_device_->IsEnabled())
    {
        return;
    }

    // Disable 3D processing for this sound
    buffers_3d_[current_buffer_]->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);

    buffers_[current_buffer_]->Play(0, 0, looped ? DSBPLAY_LOOPING : 0);

    current_buffer_++;
    if(current_buffer_ >= NUM_BUFFERS)
    {
        current_buffer_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Sound::Play3d(const Math::Vector& position, bool looped)
{
    if(!audio_device_->IsEnabled())
    {
        return;
    }

    // Add the sound to the active 3d sound list.
    Active3dSounds* active_3d_sounds = audio_device_->GetActive3dSounds();
    active_3d_sounds->Add(shared_from_this(), current_buffer_ );

    // Enable 3D processing for this sound
    buffers_3d_[current_buffer_]->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);
    buffers_3d_[current_buffer_]->SetPosition(position.x_, position.y_, position.z_, DS3D_IMMEDIATE);

    buffers_[current_buffer_]->Play(0, 0, looped ? DSBPLAY_LOOPING : 0);

    current_buffer_++;
    if(current_buffer_ >= NUM_BUFFERS)
    {
        current_buffer_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Sound::IsBufferPlaying(int buffer_index) const
{
    if(buffer_index >= 0 && buffer_index < NUM_BUFFERS)
    {
        DWORD status;
        buffers_[buffer_index]->GetStatus(&status);
        return (status & DSBSTATUS_PLAYING) != 0;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void Sound::Disable3dProcessing(int buffer_index)
{
    if(buffer_index >= 0 && buffer_index < NUM_BUFFERS)
    {
        buffers_3d_[buffer_index]->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Sound::CopyAudioData(IDirectSoundBuffer* buffer, boost::shared_array<unsigned char> bytes, int num_bytes)
{
    unsigned char* ptr1, *ptr2;
    DWORD num1, num2;

    HRESULT hr = buffer->Lock(
        0, num_bytes,
        reinterpret_cast<void**>(&ptr1), &num1,
        reinterpret_cast<void**>(&ptr2), &num2,
        DSBLOCK_FROMWRITECURSOR);
    if(FAILED(hr))
    {
        THROW_COM("Could not lock an instance of a DirectSoundBuffer object");
    }

    memcpy(ptr1, bytes.get(), num1);
    if(ptr2 && num2)
    {
        memcpy(ptr2, bytes.get() + num1, num2);
    }

    buffer->Unlock(ptr1, num1, ptr2, num2);
}

///////////////////////////////////////////////////////////////////////////////
void Sound::SetVolume(long volume)
{
    // Clamp the value
    if(volume < 0) volume = 0;
    else if(volume > 100) volume = 100;

    // Convert it to a value DirectSound expects.
    long range = DSBVOLUME_MAX - DSBVOLUME_MIN;
    float percent = float(volume) / 100.0f;
    long converted_volume = DSBVOLUME_MIN + long(float(range) * percent);

    // Apply it.
    for(int i = 0; i < NUM_BUFFERS; i++)
    {
        if(buffers_[i])
        {
            buffers_[i]->SetVolume(converted_volume);
        }
    }
}
