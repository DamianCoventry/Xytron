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
#include "sprayshootenemyentity.h"
#include "iworld.h"
#include "playerentity.h"
#include "basicshotentity.h"

///////////////////////////////////////////////////////////////////////////////
SprayShootEnemyEntity::SprayShootEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: SitAndShootEnemyEntity(event_handler, world)
, shoot_state_(SS_WAITING)
, num_shots_(6)
, spread_angle_(30.0f)
, angle_delta_(5.0f)
, current_shot_angle_(0.0f)
, shot_time_(0.0f)
, shot_timeout_(0.1f)
, spray_left_(false)
, rotate_time_(0.0f)
, rotate_frame_(0)
{
}

///////////////////////////////////////////////////////////////////////////////
void SprayShootEnemyEntity::BindResources(Util::ResourceContext* resources)
{
    imgani_shipframes_[0] = resources->FindImageAnim("Images/EnemyType2_0.tga");
    imgani_shipframes_[1] = resources->FindImageAnim("Images/EnemyType2_1.tga");
    imgani_shipframes_[2] = resources->FindImageAnim("Images/EnemyType2_2.tga");
    imgani_shipframes_[3] = resources->FindImageAnim("Images/EnemyType2_3.tga");
    imgani_shipframes_[4] = resources->FindImageAnim("Images/EnemyType2_4.tga");
    imgani_shipframes_[5] = resources->FindImageAnim("Images/EnemyType2_5.tga");
    imgani_shipframes_[6] = resources->FindImageAnim("Images/EnemyType2_6.tga");
    imgani_shipframes_[7] = resources->FindImageAnim("Images/EnemyType2_7.tga");

    snd_explosions_.resize(5);
    snd_explosions_[0] = resources->FindSound("Sounds/ShipExplosion0.wav");
    snd_explosions_[1] = resources->FindSound("Sounds/ShipExplosion1.wav");
    snd_explosions_[2] = resources->FindSound("Sounds/ShipExplosion2.wav");
    snd_explosions_[3] = resources->FindSound("Sounds/ShipExplosion3.wav");
    snd_explosions_[4] = resources->FindSound("Sounds/ShipExplosion4.wav");

    snd_shoot_ = resources->FindSound("Sounds/CannonShot.wav");
}

///////////////////////////////////////////////////////////////////////////////
void SprayShootEnemyEntity::ShootThink(float time_delta)
{
    switch(shoot_state_)
    {
    case SS_WAITING:
        shoot_time_ += time_delta;
        if(shoot_time_ >= shoot_threshold_)
        {
            shoot_time_         = 0.0f;
            shoot_threshold_    = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;
            shoot_state_        = SS_SHOOTING;
            SetupSprayShot();
        }
        break;
    case SS_SHOOTING:
        {
            if(world_->Player()->Dead() || !world_->Player()->Shootable())
            {
                shoot_state_ = SS_WAITING;
            }
            else
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(Position());
                shot->Speed(shot_speed_);
                shot->Rotation(Math::Vector(0.0f, current_shot_angle_, 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);

                snd_shoot_->Play2d();

                current_shot_angle_ += angle_delta_;

                if(++current_shot_ >= num_shots_)
                {
                    shoot_time_     = 0.0f;
                    shoot_state_    = SS_WAITING;
                }
                else
                {
                    shot_time_      = 0.0f;
                    shoot_state_    = SS_SHOTWAITING;
                }
            }
            break;
        }
    case SS_SHOTWAITING:
        shot_time_ += time_delta;
        if(shot_time_ >= shot_timeout_)
        {
            shot_time_      = 0.0f;
            shoot_state_    = SS_SHOOTING;
        }
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void SprayShootEnemyEntity::SetupSprayShot()
{
    Math::Vector to_player(world_->Player()->Position() - Position());
    to_player.Normalise();

    Math::Vector zerodegrees(0.0f, 1.0f, 0.0f);
    float dot = zerodegrees.DotProduct(to_player);

    Math::Vector up(0.0f, 0.0f, 1.0f);
    Math::Vector right(zerodegrees.CrossProduct(up));

    float angle;
    if(right.DotProduct(to_player) > 0.0f)
    {
        angle = 360.0f - (Math::RTOD*acosf(dot));
    }
    else
    {
        angle = Math::RTOD*acosf(dot);
    }

    if(spray_left_)
    {
        current_shot_angle_ = angle + (spread_angle_ / 2.0f);
        angle_delta_        = -spread_angle_ / float(num_shots_);
    }
    else
    {
        current_shot_angle_ = angle - (spread_angle_ / 2.0f);
        angle_delta_        = spread_angle_ / float(num_shots_);
    }

    current_shot_   = 0;
    spray_left_     = !spray_left_;
}

///////////////////////////////////////////////////////////////////////////////
void SprayShootEnemyEntity::Think(float time_delta)
{
    rotate_time_ += time_delta;
    if(rotate_time_ >= 0.1f)
    {
        rotate_time_ = 0.0f;
        if(++rotate_frame_ >= 8)
        {
            rotate_frame_ = 0;
        }
    }

    SitAndShootEnemyEntity::Think(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void SprayShootEnemyEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }

    float adjusted = rotation_.y_+5.0f;
    if(adjusted >= 360.0f) adjusted -= 360.0f;

    imgani_shipframes_[rotate_frame_]->CurrentFrame(int(adjusted/10.0f));
    imgani_shipframes_[rotate_frame_]->Draw2d(g, Position());
}
