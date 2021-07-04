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
#ifndef INCLUDED_AUD_DEVICE
#define INCLUDED_AUD_DEVICE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <string>
#include <vector>

#include "active3dsounds.h"

#include "../math/vector.h"

///////////////////////////////////////////////////////////////////////////////
namespace Aud
{

class Sound;
typedef boost::shared_ptr<Sound> SoundPtr;

///////////////////////////////////////////////////////////////////////////////
class Device
{
public:
    Device();
    ~Device();

    void Initialise(HWND window, GUID* guid, int sps, int bps, int channels);
    void Shutdown();

    WAVEFORMATEX* GetFormat() const             { return (WAVEFORMATEX*)&format_; }
    Active3dSounds* GetActive3dSounds() const   { return (Active3dSounds*)&active_3d_sounds_; }

    void Enable()   { enabled_ = true; }
    void Disable()  { enabled_ = false; }
    bool IsEnabled() const { return enabled_; }

    void SetListenerPosition(const Math::Vector& position,
                             const Math::Vector& forward,
                             const Math::Vector& up = Math::Vector(0.0f, 1.0f, 0.0f));

    SoundPtr CreateSound(boost::shared_array<unsigned char> bytes, int num_bytes, float min_distance, float max_distance);

    void Update3dSounds()   { active_3d_sounds_.RemoveFinishedSounds(); }
    void Clear3dSounds()    { active_3d_sounds_.Clear(); }

private:
    void CreatePrimaryBuffer();
    void SetPrimaryBufferFormat(int sps, int bps, int channels);
    void Create3dListener();

private:
    WAVEFORMATEX    format_;
    bool            enabled_;
    Active3dSounds  active_3d_sounds_;

    IDirectSound8*          dsound_;
    IDirectSoundBuffer*     primary_;
    IDirectSound3DListener* listener_;
};

}       // namespace Aud

#endif  // INCLUDED_AUD_DEVICE
