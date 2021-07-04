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
#include "basicshotentity.h"
#include "iworld.h"
#include "basicenemyentity.h"
#include "playerentity.h"
#include "basicshotexplosionentity.h"
#include "basicshottrailentity.h"
#include "iworld.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const unsigned int DAMAGE_CAUSED = 5;
}

///////////////////////////////////////////////////////////////////////////////
BasicShotEntity::BasicShotEntity(IWorld* world)
: world_(world)
, frame_time_(0.0f)
, speed_(675.0f)
, current_frame_(0)
{
    shot_trail_.reset(new BasicShotTrailEntity(world, Position()));
    BasicShotTrailEntity* explosion = static_cast<BasicShotTrailEntity*>(shot_trail_.get());
    explosion->Position(Position());
    world->ExplosionsEntityList().push_back(shot_trail_);
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotEntity::BindResources(Util::ResourceContext* resources)
{
    imgani_         = resources->FindImageAnim("Images/EnemyShot.tga");
    snd_collide0_   = resources->FindSound("Sounds/CannonHit0.wav");
    snd_collide1_   = resources->FindSound("Sounds/CannonHit1.wav");
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }
    frame_time_ += time_delta;
    if(frame_time_ >= 0.05f)        // 20fps
    {
        frame_time_ = 0.0f;
        if(++current_frame_ >= imgani_->ImageCount())
        {
            current_frame_ = 0;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotEntity::Move(float time_delta)
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
void BasicShotEntity::CollideWithPlayer(Gfx::IEntityPtr player_entity)
{
    if(Dead())
    {
        return;
    }
    if(IsColliding(player_entity))
    {
        Kill();
        PlayerEntity* player = static_cast<PlayerEntity*>(player_entity.get()   );
        if(!player->TakeDamage(DAMAGE_CAUSED))
        {
            // The player didn't die from this damage.  We'll draw some sparks to
            // indicate this shot exploded.  We don't bother drawing this sparked
            // based explosion if the player did explode - there'll be enough crap
            // drawn by the player's explosion.
            Gfx::IEntityPtr entity(new BasicShotExplosionEntity(Position()));
            BasicShotExplosionEntity* explosion = static_cast<BasicShotExplosionEntity*>(entity.get());
            explosion->Position(Position());
            world_->ExplosionsEntityList().push_back(entity);

            (rand()%2 == 0) ? snd_collide0_->Play2d() : snd_collide1_->Play2d();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotEntity::CollideWithEnemyEntities(Gfx::EntityList& entities)
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
            if(!enemy->TakeDamage(DAMAGE_CAUSED))
            {
                // The enemy didn't die from this damage.  We'll draw some sparks to
                // indicate this shot exploded.  We don't bother drawing this sparked
                // based explosion if the enemy did explode - there'll be enough crap
                // drawn by that enemy's explosion.
                Gfx::IEntityPtr entity(new BasicShotExplosionEntity(Position()));
                BasicShotExplosionEntity* explosion = static_cast<BasicShotExplosionEntity*>(entity.get());
                explosion->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);

                (rand()%2 == 0) ? snd_collide0_->Play2d() : snd_collide1_->Play2d();
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotEntity::Kill()
{
    BasicShotTrailEntity* explosion = static_cast<BasicShotTrailEntity*>(shot_trail_.get());
    explosion->OwnerDeath();
    IEntity::Kill();
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }
    imgani_->CurrentFrame(current_frame_);
    imgani_->Draw2d(g, Position());
}
