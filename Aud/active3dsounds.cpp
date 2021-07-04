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
#include "active3dsounds.h"
#include "sound.h"

#include "../util/throw.h"

using namespace Aud;

///////////////////////////////////////////////////////////////////////////////
Active3dSounds::Active3dSounds()
{
}

///////////////////////////////////////////////////////////////////////////////
void Active3dSounds::Add(SoundPtr sound, int buffer_index)
{
    ActiveSoundPtr active_sound(new ActiveSound);
    active_sound->sound_        = sound;
    active_sound->buffer_index_ = buffer_index;
    active_sounds_.push_back(active_sound);
}

///////////////////////////////////////////////////////////////////////////////
void Active3dSounds::Clear()
{
    active_sounds_.clear();
}

///////////////////////////////////////////////////////////////////////////////
void Active3dSounds::RemoveFinishedSounds()
{
    ActiveSounds::iterator current = active_sounds_.begin();
    while(current != active_sounds_.end())
    {
        ActiveSounds::iterator prev = current;
        ++current;

        if(!(*prev)->sound_->IsBufferPlaying((*prev)->buffer_index_))
        {
            active_sounds_.erase(prev);
        }
    }
}
