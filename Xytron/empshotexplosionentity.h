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
#ifndef INCLUDED_EMPSHOTEXPLOSIONENTITY
#define INCLUDED_EMPSHOTEXPLOSIONENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class EmpShotExplosionEntity
    : public Gfx::IEntity
{
public:
    EmpShotExplosionEntity(const Math::Vector& position, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    virtual void Think(float time_delta);
    virtual void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 1.0f; }
    float BbHeight() const      { return 1.0f; }
    float BbHalfWidth() const   { return 0.5f; }
    float BbHalfHeight() const  { return 0.5f; }

protected:
    IWorld* world_;
    bool state_pre_fade_;
    float pre_fade_time_;
    float wake_size_;
    float take_damage_time_;
    Gfx::ParticleImage wake_;
};

#endif  // INCLUDED_EMPSHOTEXPLOSIONENTITY
