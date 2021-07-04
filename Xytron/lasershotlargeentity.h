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
#ifndef INCLUDED_LASERSHOTLARGEENTITY
#define INCLUDED_LASERSHOTLARGEENTITY

#include "../gfx/gfx.h"
#include "../aud/aud.h"
#include "../util/resourcecontext.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class LaserShotLargeEntity
    : public Gfx::IEntity
{
public:
    LaserShotLargeEntity(IWorld* world);

    void Rotation(const Math::Vector& rotation) { rotation_ = rotation; }

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Move(float time_delta);
    void CollideWithEnemyEntities(Gfx::EntityList& entities);

    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 45.0f; }
    float BbHeight() const      { return 45.0f; }
    float BbHalfWidth() const   { return 22.5f; }
    float BbHalfHeight() const  { return 22.5f; }

private:
    IWorld*             world_;
    Gfx::ImagePtr       img_;
    Aud::SoundPtr       snd_collide_;
    Math::Vector        rotation_;

    float speed_;
};

#endif  // INCLUDED_LASERSHOTLARGEENTITY
