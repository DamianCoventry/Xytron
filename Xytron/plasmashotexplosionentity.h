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
#ifndef INCLUDED_PLASMASHOTEXPLOSIONENTITY
#define INCLUDED_PLASMASHOTEXPLOSIONENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

///////////////////////////////////////////////////////////////////////////////
class PlasmaShotExplosionEntity
    : public Gfx::IEntity
{
public:
    PlasmaShotExplosionEntity(const Math::Vector& position, bool green);

    void BindResources(Util::ResourceContext* resources);

    virtual void Think(float time_delta);
    virtual void Move(float time_delta);
    virtual void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 1.0f; }
    float BbHeight() const      { return 1.0f; }
    float BbHalfWidth() const   { return 0.5f; }
    float BbHalfHeight() const  { return 0.5f; }

protected:
    bool state_pre_fade_;
    bool green_;
    int num_sparks_;
    float pre_fade_time_;
    float wake_size_;
    Gfx::ParticlePixelList  sparks_;
    Gfx::ParticleImage      wake_;
};

#endif  // INCLUDED_PLASMASHOTEXPLOSIONENTITY
