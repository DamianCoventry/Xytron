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
#include "nukeshotentity.h"
#include "iworld.h"
#include "nukeexplosionentity.h"
#include "basicenemyentity.h"
#include "basicshotexplosionentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float MAX_DAMAGE = 80.0f;
}

///////////////////////////////////////////////////////////////////////////////
NukeShotEntity::NukeShotEntity(IWorld* world)
: world_(world)
, frame_time_(0.0f)
, current_frame_(0)
, speed_(640.0f)
, blast_radius_(256.0f)
{
    blast_radius_squared_ = blast_radius_*blast_radius_;
}

///////////////////////////////////////////////////////////////////////////////
void NukeShotEntity::BindResources(Util::ResourceContext* resources)
{
    imgani_         = resources->FindImageAnim("Images/NukeShot.tga");
    snd_explode_    = resources->FindSound("Sounds/NukeExplosion.wav");
}

///////////////////////////////////////////////////////////////////////////////
void NukeShotEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }
    frame_time_ += time_delta;
    if(frame_time_ >= 0.1f)
    {
        frame_time_ = 0.0f;

        if(++current_frame_ >= imgani_->ImageCount())
        {
            float x_delta, y_delta;
            float distance, percent_total_damage;
            Gfx::EntityList::iterator itor;

            // Assign damage to each enemy ship
            for(itor = world_->EnemyEntityList().begin(); itor != world_->EnemyEntityList().end(); ++itor)
            {
                x_delta = (*itor)->Position().x_ - Position().x_;
                y_delta = (*itor)->Position().y_ - Position().y_;
                distance = static_cast<float>(sqrt((x_delta * x_delta) + (y_delta * y_delta)));
                if(distance < blast_radius_)
                {
                    percent_total_damage = 1.0f - (distance / blast_radius_);
                    BasicEnemyEntity* enemy = static_cast<BasicEnemyEntity*>(itor->get());
                    if(!enemy->TakeDamage(unsigned int(MAX_DAMAGE * percent_total_damage)))
                    {
                        // The enemy didn't die from this bomb.  We'll draw some sparks to
                        // indicate this enemy took damage.  We don't bother drawing this sparked
                        // based explosion if the enemy did explode - there'll be enough crap
                        // drawn by that enemy's explosion.
                        Gfx::IEntityPtr entity(new BasicShotExplosionEntity(Position()));
                        BasicShotExplosionEntity* explosion = static_cast<BasicShotExplosionEntity*>(entity.get());
                        explosion->Position(Position());
                        world_->ExplosionsEntityList().push_back(entity);
                    }
                }
            }

            // Kill any enemy shots within the blast radius.
            for(itor = world_->EnemyShotsEntityList().begin(); itor != world_->EnemyShotsEntityList().end(); ++itor)
            {
                x_delta = (*itor)->Position().x_ - Position().x_;
                y_delta = (*itor)->Position().y_ - Position().y_;
                distance = (x_delta*x_delta) + (y_delta*y_delta);
                // We can avoid a call to sqrt() here if we note that x^2 + y^2 = z^2
                if(distance < blast_radius_squared_)
                {
                    (*itor)->Kill();
                }
            }

            // Create a nuke explosion entity
            Gfx::IEntityPtr entity(new NukeExplosionEntity(Position()));
            NukeExplosionEntity* explosion = static_cast<NukeExplosionEntity*>(entity.get());
            explosion->Position(Position());
            explosion->BindResources(world_->ResourceContext());
            world_->ExplosionsEntityList().push_back(entity);

            snd_explode_->Play2d();

            Kill();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void NukeShotEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    if(speed_ > 0.0f)
    {
        Math::Vector pos(Position());
        pos -= Math::Vector(0.0f, speed_*time_delta, 0.0f);
        Position(pos);
        speed_ *= 0.95f;
    }
}

///////////////////////////////////////////////////////////////////////////////
void NukeShotEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }
    imgani_->CurrentFrame(current_frame_);
    imgani_->Draw2d(g, Position());
}
