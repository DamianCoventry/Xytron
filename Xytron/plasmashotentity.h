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
#ifndef INCLUDED_PLASMASHOTENTITY
#define INCLUDED_PLASMASHOTENTITY

#include "../gfx/gfx.h"
#include "../aud/aud.h"
#include "../util/resourcecontext.h"

class PlayerEntity;
struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class PlasmaShotEntity
    : public Gfx::IEntity
{
public:
    PlasmaShotEntity(IWorld* world);

    void Rotation(const Math::Vector& rotation) { rotation_ = rotation; }

    void BindResources(Util::ResourceContext* resources, unsigned int upgrade);

    void Move(float time_delta);
    void CollideWithPlayer(Gfx::IEntityPtr player_entity);
    void CollideWithEnemyEntities(Gfx::EntityList& entities);

    void Speed(float speed) { speed_ = speed; }

    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const;
    float BbHeight() const;
    float BbHalfWidth() const;
    float BbHalfHeight() const;

private:
    IWorld*         world_;
    Gfx::ImagePtr   img_;
    Math::Vector    rotation_;
    Aud::SoundPtr   snd_collide_;

    unsigned int damage_;
    unsigned int upgrade_;
    float speed_;
};

#endif  // INCLUDED_PlasmaSHOTENTITY
