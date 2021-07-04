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
#include "empshotexplosionentity.h"
#include "basicenemyentity.h"
#include "basicshotexplosionentity.h"
#include "iworld.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float PRE_FADE_THRESHOLD      = 0.5f;
    const float ALPHA_THRESHOLD         = 0.1f;    // 10fps
    const float INNER_OUTER_WAVE_DELTA  = 32.0f;
    const unsigned int EMP_DAMAGE       = 3;
}

///////////////////////////////////////////////////////////////////////////////
EmpShotExplosionEntity::EmpShotExplosionEntity(const Math::Vector& position, IWorld* world)
: state_pre_fade_(true)
, world_(world)
, pre_fade_time_(0.0f)
, wake_size_(1.0f)
, take_damage_time_(0.0f)
{
    wake_.alpha_inc_        = -0.1f;
    wake_.alpha_threshold_  = ALPHA_THRESHOLD;
    wake_.position_         = position;
}

///////////////////////////////////////////////////////////////////////////////
void EmpShotExplosionEntity::BindResources(Util::ResourceContext* resources)
{
    wake_.image_ = resources->FindImage("Images/WakeEmp.tga");
}

///////////////////////////////////////////////////////////////////////////////
void EmpShotExplosionEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }

    wake_size_ += 450.0f * time_delta;

    if(state_pre_fade_)
    {
        pre_fade_time_ += time_delta;
        if(pre_fade_time_ >= PRE_FADE_THRESHOLD)
        {
            state_pre_fade_ = false;
        }
    }
    else
    {
        wake_.ThinkAlpha(time_delta);

        if(wake_size_ >= 800.0f || wake_.alpha_ <= 0.0f)
        {
            Kill();
        }
    }

    take_damage_time_ += time_delta;
    if(take_damage_time_ >= 0.050f)     // Assign damage 20 times per second
    {
        take_damage_time_ = 0.0f;

        float x_delta, y_delta;
        float distance;
        float outer_blast_radius    = wake_size_/2.0f;
        float inner_blast_radius    = outer_blast_radius - INNER_OUTER_WAVE_DELTA;
        Gfx::EntityList::iterator itor;

        // Assign damage to each enemy ship
        for(itor = world_->EnemyEntityList().begin(); itor != world_->EnemyEntityList().end(); ++itor)
        {
            x_delta = (*itor)->Position().x_ - wake_.position_.x_;
            y_delta = (*itor)->Position().y_ - wake_.position_.y_;
            distance = sqrt((x_delta*x_delta) + (y_delta*y_delta));
            if(distance < outer_blast_radius && distance > inner_blast_radius)
            {
                BasicEnemyEntity* enemy = static_cast<BasicEnemyEntity*>(itor->get());
                if(!enemy->TakeDamage(EMP_DAMAGE))
                {
                    // The enemy didn't die from this bomb.  We'll draw some sparks to
                    // indicate this enemy took damage.  We don't bother drawing this sparked
                    // based explosion if the enemy did explode - there'll be enough crap
                    // drawn by that enemy's explosion.
                    Gfx::IEntityPtr entity(new BasicShotExplosionEntity(enemy->Position()));
                    BasicShotExplosionEntity* explosion = static_cast<BasicShotExplosionEntity*>(entity.get());
                    explosion->Position(enemy->Position());
                    world_->ExplosionsEntityList().push_back(entity);
                }
            }
        }

        // Kill any enemy shots within the blast radius.
        for(itor = world_->EnemyShotsEntityList().begin(); itor != world_->EnemyShotsEntityList().end(); ++itor)
        {
            x_delta = (*itor)->Position().x_ - wake_.position_.x_;
            y_delta = (*itor)->Position().y_ - wake_.position_.y_;
            distance = sqrt((x_delta*x_delta) + (y_delta*y_delta));
            if(distance < outer_blast_radius && distance > inner_blast_radius)
            {
                (*itor)->Kill();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void EmpShotExplosionEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }

    if(wake_.alpha_ > 0.0f)
    {
        g.Draw2dQuad(
            wake_.image_->Surface(),
            wake_.position_ - Math::Vector(wake_size_/2.0f, wake_size_/2.0f, 0.0f),
            wake_size_, wake_size_, wake_.image_->Tc0(), wake_.image_->Tc1(), wake_.alpha_);

        //wake_.image_->Draw2d(g, central_flash_.position_, wake_.alpha_);
    }
}
