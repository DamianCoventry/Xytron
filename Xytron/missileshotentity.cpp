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
#include "stdafx.h"
#include "missileshotentity.h"
#include "iworld.h"
#include "basicenemyentity.h"
#include "missileshotexplosionentity.h"
#include "missileshottrailentity.h"
#include "playerentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const unsigned int DAMAGE_CAUSED = 5;
}

///////////////////////////////////////////////////////////////////////////////
MissileShotEntity::MissileShotEntity(IWorld* world, bool up)
: world_(world)
, frame_time_(0.0f)
, speed_(500.0f)
, current_frame_(0)
, up_(up)
{
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotEntity::BindResources(Util::ResourceContext* resources)
{
    imgani_ = resources->FindImageAnim(up_ ? "Images/MissileShotUp.tga" : "Images/MissileShotDown.tga");

    shot_trail_.reset(new MissileShotTrailEntity(world_, Position()));
    MissileShotTrailEntity* trail = static_cast<MissileShotTrailEntity*>(shot_trail_.get());
    trail->BindResources(world_->ResourceContext());
    trail->Position(Position());
    world_->ExplosionsEntityList().push_back(shot_trail_);

    snd_collide_ = resources->FindSound("Sounds/MissilesHit.wav");
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }
    frame_time_ += time_delta;
    if(frame_time_ >= 0.1f)        // 10fps
    {
        frame_time_ = 0.0f;
        if(++current_frame_ >= imgani_->ImageCount())
        {
            current_frame_ = 0;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    Math::Vector pos = Position();
    pos.x_ += (speed_*time_delta) * -sinf(rotation_.y_*Math::DTOR);
    pos.y_ += (speed_*time_delta) * cosf(rotation_.y_*Math::DTOR);
    Position(pos);

    if(Position().x_ < 100.0f || Position().x_ >= 700.0f ||
       Position().y_ < 0.0f || Position().y_ >= 600.0f)
    {
        Kill();
    }
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotEntity::CollideWithPlayer(Gfx::IEntityPtr player_entity)
{
    if(Dead())
    {
        return;
    }

    if(IsColliding(player_entity))
    {
        OnCollide(player_entity);

        PlayerEntity* player = static_cast<PlayerEntity*>(player_entity.get());
        player->TakeDamage(DAMAGE_CAUSED);

        Gfx::IEntityPtr entity(new MissileShotExplosionEntity(Position()));
        MissileShotExplosionEntity* explosion = static_cast<MissileShotExplosionEntity*>(entity.get());
        explosion->BindResources(world_->ResourceContext());
        explosion->Position(Position());
        world_->ExplosionsEntityList().push_back(entity);

        snd_collide_->Play2d();
    }
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotEntity::CollideWithEnemyEntities(Gfx::EntityList& entities)
{
    if(Dead())
    {
        return;
    }

    Gfx::EntityList::iterator itor;
    for(itor = entities.begin(); itor != entities.end(); ++itor)
    {
        if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f) && IsColliding(*itor))
        {
            OnCollide(*itor);

            BasicEnemyEntity* enemy = static_cast<BasicEnemyEntity*>(itor->get());
            enemy->TakeDamage(DAMAGE_CAUSED);

            Gfx::IEntityPtr entity(new MissileShotExplosionEntity(Position()));
            MissileShotExplosionEntity* explosion = static_cast<MissileShotExplosionEntity*>(entity.get());
            explosion->BindResources(world_->ResourceContext());
            explosion->Position(Position());
            world_->ExplosionsEntityList().push_back(entity);

            snd_collide_->Play2d();
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotEntity::Kill()
{
    MissileShotTrailEntity* trail = static_cast<MissileShotTrailEntity*>(shot_trail_.get());
    trail->OwnerDeath();
    IEntity::Kill();
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }
    imgani_->CurrentFrame(current_frame_);
    imgani_->Draw2d(g, Position());
}
