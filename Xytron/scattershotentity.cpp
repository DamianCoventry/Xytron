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
#include "scattershotentity.h"
#include "iworld.h"
#include "scatterexplosionentity.h"
#include "basicenemyentity.h"
#include "basicshotexplosionentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float MAX_DAMAGE  = 20.0f;
}

///////////////////////////////////////////////////////////////////////////////
ScatterShotEntity::ScatterShotEntity(IWorld* world)
: world_(world)
, blast_radius_(100.0f)
{
    blast_radius_squared_ = blast_radius_*blast_radius_;
}

///////////////////////////////////////////////////////////////////////////////
void ScatterShotEntity::BindResources(Util::ResourceContext* resources)
{
    bomb_.current_frame_    = 0;
    bomb_.imgani_time_      = 0.0f;
    bomb_.imgani_threshold_ = 0.016f + (float(rand()%10)/1000.0f);
    bomb_.speed_            = float((rand()%150)+200);
    bomb_.rotation_         = float(rand()%360);
    bomb_.imgani_           = world_->ResourceContext()->FindImageAnim("Images/ScatterShot.tga");

    snd_explosions_.resize(5);
    snd_explosions_[0] = resources->FindSound("Sounds/ShipExplosion0.wav");
    snd_explosions_[1] = resources->FindSound("Sounds/ShipExplosion1.wav");
    snd_explosions_[2] = resources->FindSound("Sounds/ShipExplosion2.wav");
    snd_explosions_[3] = resources->FindSound("Sounds/ShipExplosion3.wav");
    snd_explosions_[4] = resources->FindSound("Sounds/ShipExplosion4.wav");
}

///////////////////////////////////////////////////////////////////////////////
void ScatterShotEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }

    bomb_.imgani_time_ += time_delta;
    if(bomb_.imgani_time_ >= bomb_.imgani_threshold_)
    {
        bomb_.imgani_time_ = 0.0f;

        if(++bomb_.current_frame_ >= bomb_.imgani_->ImageCount())
        {
            float x_delta, y_delta;
            float distance, percent_total_damage;
            Gfx::EntityList::iterator itor_ent;

            // Assign damage to each enemy ship
            for(itor_ent = world_->EnemyEntityList().begin(); itor_ent != world_->EnemyEntityList().end(); ++itor_ent)
            {
                x_delta = (*itor_ent)->Position().x_ - Position().x_;
                y_delta = (*itor_ent)->Position().y_ - Position().y_;
                distance = static_cast<float>(sqrt((x_delta*x_delta) + (y_delta*y_delta)));
                if(distance < blast_radius_)
                {
                    percent_total_damage = 1.0f - (distance / blast_radius_);
                    BasicEnemyEntity* enemy = static_cast<BasicEnemyEntity*>(itor_ent->get());
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
            for(itor_ent = world_->EnemyShotsEntityList().begin(); itor_ent != world_->EnemyShotsEntityList().end(); ++itor_ent)
            {
                x_delta = (*itor_ent)->Position().x_ - Position().x_;
                y_delta = (*itor_ent)->Position().y_ - Position().y_;
                distance = (x_delta*x_delta) + (y_delta*y_delta);
                // We can avoid a call to sqrt() here if we note that x^2 + y^2 = z^2
                if(distance < blast_radius_squared_)
                {
                    (*itor_ent)->Kill();
                }
            }

            // Create a Scatter explosion entity
            Gfx::IEntityPtr entity(new ScatterExplosionEntity(Position()));
            ScatterExplosionEntity* explosion = static_cast<ScatterExplosionEntity*>(entity.get());
            explosion->Position(Position());
            explosion->BindResources(world_->ResourceContext());
            world_->ExplosionsEntityList().push_back(entity);

            snd_explosions_[rand()%5]->Play2d();
            Kill();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void ScatterShotEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }

    Math::Vector pos(Position());
    pos.x_ += (bomb_.speed_*time_delta) * -sinf(bomb_.rotation_*Math::DTOR);
    pos.y_ += (bomb_.speed_*time_delta) * cosf(bomb_.rotation_*Math::DTOR);
    Position(pos);
}

///////////////////////////////////////////////////////////////////////////////
void ScatterShotEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }

    bomb_.imgani_->CurrentFrame(bomb_.current_frame_);
    bomb_.imgani_->Draw2d(g, Position());
}
