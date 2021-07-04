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
#ifndef INCLUDED_BASICSHOTEXPLOSIONENTITY
#define INCLUDED_BASICSHOTEXPLOSIONENTITY

#include "../gfx/gfx.h"

///////////////////////////////////////////////////////////////////////////////
class BasicShotExplosionEntity
    : public Gfx::IEntity
{
public:
    BasicShotExplosionEntity(const Math::Vector& position,
                             int num_sparks = 10,
                             int min_r = 220, int max_r = 255,
                             int min_g = 220, int max_g = 255,
                             int min_b = 5, int max_b = 25);

    virtual void Think(float time_delta);
    virtual void Move(float time_delta);
    virtual void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 1.0f; }
    float BbHeight() const      { return 1.0f; }
    float BbHalfWidth() const   { return 0.5f; }
    float BbHalfHeight() const  { return 0.5f; }

protected:
    bool state_pre_fade_;
    float pre_fade_time_;
    int num_sparks_;
    Gfx::ParticlePixelList sparks_;
};

#endif  // INCLUDED_BASICSHOTEXPLOSIONENTITY
