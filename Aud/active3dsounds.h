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
#ifndef INCLUDED_AUD_ACTIVE3DSOUNDS
#define INCLUDED_AUD_ACTIVE3DSOUNDS

#include <list>
#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace Aud
{

class Sound;
typedef boost::shared_ptr<Sound> SoundPtr;

///////////////////////////////////////////////////////////////////////////////
class Active3dSounds
{
public:
    Active3dSounds();

    void Add(SoundPtr sound, int buffer_index);
    void Clear();
    void RemoveFinishedSounds();

private:
    struct ActiveSound
    {
        SoundPtr sound_;
        int buffer_index_;
    };
    typedef boost::shared_ptr<ActiveSound> ActiveSoundPtr;

    typedef std::list<ActiveSoundPtr> ActiveSounds;
    ActiveSounds active_sounds_;
};

}       // namespace Aud

#endif  // INCLUDED_AUD_ACTIVE3DSOUNDS
