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
#ifndef INCLUDED_BASICSHOTENTITY
#define INCLUDED_BASICSHOTENTITY

#include "../gfx/gfx.h"
#include "../aud/aud.h"
#include "../util/resourcecontext.h"

class PlayerEntity;
struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class BasicShotEntity
    : public Gfx::IEntity
{
public:
    BasicShotEntity(IWorld* world);

    void Rotation(const Math::Vector& rotation) { rotation_ = rotation; }

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Move(float time_delta);
    void CollideWithPlayer(Gfx::IEntityPtr player);
    void CollideWithEnemyEntities(Gfx::EntityList& entities);

    void Speed(float speed) { speed_ = speed; }

    void Kill();
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const { return 14.0f; }
    float BbHeight() const { return 14.0f; }
    float BbHalfWidth() const { return 7.0f; }
    float BbHalfHeight() const { return 7.0f; }

private:
    IWorld*             world_;
    Gfx::IEntityPtr     shot_trail_;
    Gfx::ImageAnimPtr   imgani_;
    Math::Vector        rotation_;
    Aud::SoundPtr       snd_collide0_;
    Aud::SoundPtr       snd_collide1_;

    int current_frame_;
    float frame_time_;
    float speed_;
};

#endif  // INCLUDED_BASICSHOTENTITY
