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
#ifndef INCLUDED_MISSILESHOTTRAILENTITY
#define INCLUDED_MISSILESHOTTRAILENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class MissileShotTrailEntity
    : public Gfx::IEntity
{
public:
    MissileShotTrailEntity(IWorld* world, const Math::Vector& owner_position);

    void BindResources(Util::ResourceContext* resources) { }
    void OwnerDeath() { owner_death_ = true; }

    virtual void Think(float time_delta);
    virtual void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 1.0f; }
    float BbHeight() const      { return 1.0f; }
    float BbHalfWidth() const   { return 0.5f; }
    float BbHalfHeight() const  { return 0.5f; }

protected:
    IWorld* world_;
    bool owner_death_;
    float emit_time_;
    const Math::Vector& owner_position_;
    Gfx::ParticleImageList images_;
    std::vector<float> sizes_;
};

#endif  // INCLUDED_MISSILESHOTTRAILENTITY
