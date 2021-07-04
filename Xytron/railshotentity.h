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
#ifndef INCLUDED_RAILSHOTENTITY
#define INCLUDED_RAILSHOTENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class RailShotEntity
    : public Gfx::IEntity
{
public:
    RailShotEntity(IWorld* world);

    void BindResources(Util::ResourceContext* resources, unsigned int upgrade);

    void Damage(unsigned int damage)    { damage_ = damage; }
    void Think(float time_delta);
    void CollideWithPlayer(Gfx::IEntityPtr player);
    void CollideWithEnemyEntities(Gfx::EntityList& entities);

    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const;
    float BbHalfWidth() const;
    float BbHeight() const      { return 125.0f; }
    float BbHalfHeight() const  { return 62.5f; }

private:
    IWorld*         world_;
    Gfx::ImagePtr   img_;
    float alpha_time_;
    float alpha_;
    unsigned int damage_;
    unsigned int upgrade_;
    bool damage_depleted_;
};

#endif  // INCLUDED_RAILSHOTENTITY
