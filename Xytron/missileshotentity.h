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
#ifndef INCLUDED_MISSILESHOTENTITY
#define INCLUDED_MISSILESHOTENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"
#include "../aud/aud.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class MissileShotEntity
    : public Gfx::IEntity
{
public:
    MissileShotEntity(IWorld* world, bool up = true);

    void Rotation(const Math::Vector& rotation) { rotation_ = rotation; }

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Move(float time_delta);
    void CollideWithEnemyEntities(Gfx::EntityList& entities);
    void CollideWithPlayer(Gfx::IEntityPtr player);

    void Speed(float speed) { speed_ = speed; }

    void Kill();
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 14.0f; }
    float BbHeight() const      { return 14.0f; }
    float BbHalfWidth() const   { return 7.0f; }
    float BbHalfHeight() const  { return 7.0f; }

private:
    IWorld*             world_;
    Gfx::IEntityPtr     shot_trail_;
    Gfx::ImageAnimPtr   imgani_;
    Math::Vector        rotation_;
    Aud::SoundPtr       snd_collide_;

    int current_frame_;
    float frame_time_;
    float speed_;
    bool up_;
};

#endif  // INCLUDED_MISSILESHOTENTITY
