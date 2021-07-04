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
#ifndef INCLUDED_SMALLENEMYEXPLOSIONENTITY
#define INCLUDED_SMALLENEMYEXPLOSIONENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

///////////////////////////////////////////////////////////////////////////////
class SmallEnemyExplosionEntity
    : public Gfx::IEntity
{
public:
    SmallEnemyExplosionEntity(const Math::Vector& position);

    void BindResources(Util::ResourceContext* resources);

    virtual void Think(float time_delta);
    virtual void Move(float time_delta);
    virtual void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 20.0f; }
    float BbHeight() const      { return 20.0f; }
    float BbHalfWidth() const   { return 10.0f; }
    float BbHalfHeight() const  { return 10.0f; }

protected:
    bool state_pre_fade_;
    float pre_fade_time_;
    float wake_size_;
    Gfx::ParticleImage           central_flash_;
    Gfx::ParticleImage           wake_;
    Gfx::ParticleImageAnimList   debris_;
    Gfx::ParticleImageList       big_sparks_;
    Gfx::ParticlePixelList       small_sparks_;
};

#endif  // INCLUDED_SMALLENEMYEXPLOSIONENTITY
