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
#ifndef INCLUDED_AUD_MUSIC
#define INCLUDED_AUD_MUSIC

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <strmif.h>
#include <uuids.h>
#include <control.h>
#include <evcode.h>

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace Aud
{

///////////////////////////////////////////////////////////////////////////////
class Music
{
public:
    Music();
    ~Music() { Stop(); Unload(); }

    void Enable()   { enabled_ = true; }
    void Disable()  { Stop(); enabled_ = false; }
    bool IsEnabled() const { return enabled_; }

    void Load(const std::string& filename, HWND window, int window_message_id);
    void Unload();
    void Play();
    void Stop();
    void Pause();
    void Resume();

    void Volume(long volume);
    long Volume() const;

    bool OnGraphEvent();

private:
    bool enabled_;
    std::string filename_;
    IGraphBuilder*  graph_builder_;
    IMediaControl*  media_control_;
    IMediaEventEx*  media_event_;
    IMediaPosition* media_position_;
    IBasicAudio*    basic_audio_;
};

typedef boost::shared_ptr<Music> MusicPtr;
typedef std::map<std::string, MusicPtr> MusicList;

}       // namespace Aud

#endif  // INCLUDED_AUD_MUSIC
