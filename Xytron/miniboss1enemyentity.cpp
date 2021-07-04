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
#include "miniboss1enemyentity.h"
#include "basicshotentity.h"
#include "iworld.h"
#include "playerentity.h"
#include "plasmashotentity.h"
#include "nukeexplosionentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float ROTATE_THRESHOLD = 0.25f;
}

///////////////////////////////////////////////////////////////////////////////
float MiniBoss1EnemyEntity::distances_[6] = { -33.0f, -20.0f, -7.0f, 7.0f, 20.0f, 33.0f };

///////////////////////////////////////////////////////////////////////////////
MiniBoss1EnemyEntity::MiniBoss1EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
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
, num_sprays_(0)
, gun_index_(0)
, gun_index_up_(true)
, cannon_time_(0.0f)
, cannon_threshold_(0.0f)
{
    cannon_threshold_ = 2.0f + (float(rand()%500 - 250) / 1000.0f);
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss1EnemyEntity::BindResources(Util::ResourceContext* resources)
{
    imgani_ship_    = resources->FindImageAnim("Images/MiniBoss1.tga");
    snd_plasma_     = resources->FindSound("Sounds/PlasmaShot.wav");
    snd_shoot_      = resources->FindSound("Sounds/CannonShot.wav");
    snd_explode_    = resources->FindSound("Sounds/NukeExplosion.wav");
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss1EnemyEntity::ShootThink(float time_delta)
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
            num_sprays_         = 0;
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
                float adjusted = rotation_.y_+5.0f;
                if(adjusted >= 360.0f) adjusted -= 360.0f;

                float x_offset = distances_[gun_index_] * cosf(adjusted * Math::DTOR);
                float y_offset = distances_[gun_index_] * sinf(adjusted * Math::DTOR);

                Gfx::IEntityPtr entity(new PlasmaShotEntity(world_));
                PlasmaShotEntity* shot = static_cast<PlasmaShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext(), 2);
                shot->Position(Position() + Math::Vector(x_offset, y_offset, 0.0f));
                shot->Speed(shot_speed_);
                shot->Rotation(Math::Vector(0.0f, CalcAngleToPlayer() + current_shot_angle_, 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);

                if(gun_index_up_)
                {
                    if(++gun_index_ >= 6)
                    {
                        gun_index_ = 5;
                        gun_index_up_ = false;
                    }
                }
                else
                {
                    if(--gun_index_ < 0)
                    {
                        gun_index_ = 0;
                        gun_index_up_ = true;
                    }
                }

                snd_plasma_->Play2d();

                current_shot_angle_ += angle_delta_;

                if(++current_shot_ >= num_shots_)
                {
                    if(++num_sprays_ > 4)
                    {
                        num_sprays_     = 0;
                        shoot_time_     = 0.0f;
                        shoot_state_    = SS_WAITING;
                    }
                    else
                    {
                        SetupSprayShot();
                        shot_time_      = 0.0f;
                        shoot_state_    = SS_SHOTWAITING;
                    }
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
void MiniBoss1EnemyEntity::SetupSprayShot()
{
    float angle = CalcAngleToPlayer();

    if(spray_left_)
    {
        current_shot_angle_ = spread_angle_ / 2.0f;
        angle_delta_        = -spread_angle_ / float(num_shots_);
    }
    else
    {
        current_shot_angle_ = -spread_angle_ / 2.0f;
        angle_delta_        = spread_angle_ / float(num_shots_);
    }

    current_shot_   = 0;
    spray_left_     = !spray_left_;
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss1EnemyEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }
    switch(state_)
    {
    case STATE_STARTTIMEOUT:
        wait_time_ += time_delta;
        if(wait_time_ >= wait_threshold_)
        {
            state_ = STATE_DECIDING;
        }
        break;
    case STATE_DECIDING:
        turning_right_      = ClassifyRotationDirection(way_points_[current_way_point_]);
        state_              = STATE_TURNING;
        waypoint_threshold_ = 0.0f;
        break;
    case STATE_TURNING:
        {
            // Rotate towards the next waypoint
            if(turning_right_)
            {
                rotation_.y_ += rotate_speed_*time_delta;
                if(rotation_.y_ >= 360.0f) rotation_.y_ -= 360.0f;
            }
            else
            {
                rotation_.y_ -= rotate_speed_*time_delta;
                if(rotation_.y_ < 0.0f) rotation_.y_ += 360.0f;
            }

            // Are we facing it yet?
            if(turning_right_ != ClassifyRotationDirection(way_points_[current_way_point_]))
            {
                state_ = STATE_MOVING;
            }
            break;
        }
    case STATE_MOVING:
        {
            Math::Vector to_waypoint(way_points_[current_way_point_] - Position());
            float mag = to_waypoint.Magnitude();
            if(mag <= 50.0f)
            {
                if(++current_way_point_ >= int(way_points_.size()))
                {
                    current_way_point_ = 1;     // i.e. the first on screen point
                }

                sit_time_       = 0.0f;
                sit_threshold_  = float(min_sit_timeout_ + (rand()%(max_sit_timeout_ - min_sit_timeout_)))/1000.0f;
                state_          = STATE_SITTING;
                rotate_time_    = 0.0f;
                rotate_dir_     = ClassifyRotationDirection(world_->Player()->Position());
                facing_player_  = false;
            }
            else
            {
                // If it's taken more than 2 seconds to reach the next waypoint then
                // we go back to the deciding state and point the entity at the waypoint
                waypoint_threshold_ += time_delta;
                if(waypoint_threshold_ >= 2.0f)
                {
                    state_ = STATE_DECIDING;
                }
                else if(mag >= 2000.0f)
                {
                    Kill();
                    event_handler_->OnEnemyExpired(shared_from_this());
                    OutputDebugString("A BasicEnemyEntity was way off track, so it was killed\n");
                }
            }
            break;
        }
    case STATE_SITTING:
        sit_time_ += time_delta;
        if(sit_time_ >= sit_threshold_)
        {
            state_ = STATE_DECIDING;
        }
        else
        {
            if(facing_player_)
            {
                rotate_time_ += time_delta;
                if(rotate_time_ >= ROTATE_THRESHOLD)
                {
                    rotate_time_    = 0.0f;
                    rotate_dir_     = ClassifyRotationDirection(world_->Player()->Position());
                    facing_player_  = false;
                }
            }
            else
            {
                if(rotate_dir_)
                {
                    rotation_.y_ += rotate_speed_*time_delta;
                    if(rotation_.y_ >= 360.0f) rotation_.y_ -= 360.0f;
                }
                else
                {
                    rotation_.y_ -= rotate_speed_*time_delta;
                    if(rotation_.y_ < 0.0f) rotation_.y_ += 360.0f;
                }

                if(rotate_dir_ != ClassifyRotationDirection(world_->Player()->Position()))
                {
                    facing_player_ = true;
                }
            }

            if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f))
            {
                ShootThink(time_delta);
            }
        }
        break;
    }

    if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f))
    {
        cannon_time_ += time_delta;
        if(cannon_time_ >= cannon_threshold_)
        {
            cannon_time_        = 0.0f;
            cannon_threshold_   = 2.0f + (float(rand()%500 - 250) / 1000.0f);

            float angle = float(rand()%360);
            for(unsigned int i = 0; i < 12; i++)
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(Position());
                shot->Speed(shot_speed_);
                shot->Rotation(Math::Vector(0.0f, angle, 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);

                angle += 30.0f;
            }

            snd_shoot_->Play2d();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss1EnemyEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }

    float adjusted = rotation_.y_+5.0f;
    if(adjusted >= 360.0f) adjusted -= 360.0f;

    imgani_ship_->CurrentFrame(int(adjusted/10.0f));
    imgani_ship_->Draw2d(g, Position());
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss1EnemyEntity::Kill()
{
    world_->StartScreenFlash(Gfx::Color(1.0f, 1.0f, 1.0f), 3000);

    Gfx::IEntityPtr entity(new NukeExplosionEntity(Position()));
    NukeExplosionEntity* explosion = static_cast<NukeExplosionEntity*>(entity.get());
    explosion->Position(Position());
    explosion->BindResources(world_->ResourceContext());
    world_->ExplosionsEntityList().push_back(entity);

    IEntity::Kill();
}

///////////////////////////////////////////////////////////////////////////////
bool MiniBoss1EnemyEntity::TakeDamage(unsigned int damage)
{
    if(damage >= hit_points_)
    {
        hit_points_ = 0;
        snd_explode_->Play2d();
        Kill();
        event_handler_->OnEnemyDestroyed(shared_from_this());
        return true;
    }
    hit_points_ -= damage;
    return false;
}

///////////////////////////////////////////////////////////////////////////////
float MiniBoss1EnemyEntity::CalcAngleToPlayer() const
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

    return angle;
}
