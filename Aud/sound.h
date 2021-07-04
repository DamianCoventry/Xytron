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
#ifndef INCLUDED_AUD_SOUND
#define INCLUDED_AUD_SOUND

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include <string>
#include <map>

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace Math
{
struct Vector;
}

///////////////////////////////////////////////////////////////////////////////
namespace Aud
{

class Device;

class Sound;
typedef boost::shared_ptr<Sound> SoundPtr;

typedef std::map<std::string, SoundPtr> SoundList;

///////////////////////////////////////////////////////////////////////////////
class Sound
    : public boost::enable_shared_from_this<Sound>
{
public:
    static SoundPtr CreateFromFile(const std::string& filename, Device* device, float min_distance, float max_distance);

public:
    ~Sound();

    void Delete();
    void SetVolume(long volume);

    void Play2d(bool looped = false);
    void Play3d(const Math::Vector& position, bool looped = false);

    bool IsBufferPlaying(int buffer_index) const;
    void Disable3dProcessing(int buffer_index);

    float GetMinDistance() const { return min_distance_; }
    float GetMaxDistance() const { return max_distance_; }

private:
    friend class Device;

    Sound(Device* audio_device);
    void Create(IDirectSound8* dsound, boost::shared_array<unsigned char> bytes, int num_bytes, float min_distance, float max_distance);
    void CopyAudioData(IDirectSoundBuffer* buffer, boost::shared_array<unsigned char> bytes, int num_bytes);

private:
    Device*        audio_device_;
    mutable int         current_buffer_;
    float               min_distance_;
    float               max_distance_;

    enum CONSTANTS { NUM_BUFFERS = 8 };
    IDirectSoundBuffer*     buffers_[NUM_BUFFERS];
    IDirectSound3DBuffer*   buffers_3d_[NUM_BUFFERS];
};

}       // namespace Aud

#endif  // INCLUDED_AUD_SOUND
