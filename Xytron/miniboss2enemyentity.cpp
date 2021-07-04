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
#include "miniboss2enemyentity.h"
#include "iworld.h"
#include "playerentity.h"
#include "railshotentity.h"
#include "nukeexplosionentity.h"
#include "plasmashotentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float ROTATE_THRESHOLD = 0.25f;
}

///////////////////////////////////////////////////////////////////////////////
float MiniBoss2EnemyEntity::frame_shooting_offsets_[16] =
{ 60.0f, 29.0f, 29.0f, 30.0f, 32.0f, 33.0f, 36.0f, 38.0f,
  41.0f, 44.0f, 47.0f, 50.0f, 53.0f, 56.0f, 58.0f, 61.0f };

///////////////////////////////////////////////////////////////////////////////
MiniBoss2EnemyEntity::MiniBoss2EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: BasicEnemyEntity(event_handler, world)
, rotate_time_(0.0f)
, rotate_dir_(false)
, facing_player_(false)
, rotation_y_(0.0f)
, shoot_frame_(0)
, shoot_time_(0.0f)
, shoot_state_(SS_RAIL)
, num_shots_(0)
, plasma_reload_time_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss2EnemyEntity::BindResources(Util::ResourceContext* resources)
{
    snd_explode_    = resources->FindSound("Sounds/NukeExplosion.wav");
    snd_shoot_      = resources->FindSound("Sounds/RailShot.wav");
    snd_plasma_     = resources->FindSound("Sounds/PlasmaShot.wav");
    imgani_ship_    = resources->FindImageAnim("Images/MiniBoss2.tga");
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss2EnemyEntity::Think(float time_delta)
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
                state_ = STATE_DECIDING;
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
    }

    if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f))
    {
        ShootThink(time_delta);
    }

    if(facing_player_)
    {
        rotate_time_ += time_delta;
        if(rotate_time_ >= ROTATE_THRESHOLD)
        {
            rotate_time_    = 0.0f;
            facing_player_  = false;
            rotate_dir_     = ClassifyRotationDirection2(world_->Player()->Position());
        }
    }
    else
    {
        if(rotate_dir_)
        {
            rotation_y_ += rotate_speed_*time_delta;
            if(rotation_y_ >= 360.0f) rotation_y_ -= 360.0f;
        }
        else
        {
            rotation_y_ -= rotate_speed_*time_delta;
            if(rotation_y_ < 0.0f) rotation_y_ += 360.0f;
        }

        if(rotate_dir_ != ClassifyRotationDirection2(world_->Player()->Position()))
        {
            facing_player_ = true;
        }
    }

    if(shoot_frame_ > 0)
    {
        shoot_time_ += time_delta;
        if(shoot_time_ >= 0.2f)
        {
            shoot_time_ = 0.0f;
            if(++shoot_frame_ >= 16)
            {
                shoot_frame_ = 0;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss2EnemyEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }
    imgani_ship_->CurrentFrame(shoot_frame_);
    imgani_ship_->Draw2d(g, Position());
}

///////////////////////////////////////////////////////////////////////////////
bool MiniBoss2EnemyEntity::ClassifyRotationDirection2(const Math::Vector& point) const
{
    Math::Vector to_waypoint(point - Position());

    Math::Vector forwards;
    forwards.x_ = -sinf(rotation_y_*Math::DTOR);
    forwards.y_ = cosf(rotation_y_*Math::DTOR);

    Math::Vector up(0.0f, 0.0f, 1.0f);
    Math::Vector right(forwards.CrossProduct(up));

    float dot = right.DotProduct(to_waypoint);
    return (dot < 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss2EnemyEntity::ShootThink(float time_delta)
{
    switch(shoot_state_)
    {
    case SS_RAIL:
        {
            shoot_time_ += time_delta;
            if(shoot_time_ >= shoot_threshold_)
            {
                shoot_time_ = 0.0f;
                shoot_threshold_ = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;

                if(!world_->Player()->Dead() && world_->Player()->Shootable())
                {
                    snd_shoot_->Play2d();
                    shoot_frame_ = 1;

                    Gfx::ImagePtr image = world_->ResourceContext()->FindImage("Images/RailShot0.tga");

                    float rail_shot_half_height = image->FrameHeight() / 2.0f;
                    float y = Position().y_ + frame_shooting_offsets_[shoot_frame_] + rail_shot_half_height;

                    while(y < 662.5f)
                    {
                        Gfx::IEntityPtr entity(new RailShotEntity(world_));
                        RailShotEntity* shot = static_cast<RailShotEntity*>(entity.get());
                        shot->BindResources(world_->ResourceContext(), 2);
                        shot->Damage(3);
                        shot->Position(Math::Vector(Position().x_, y, 0.0f));
                        world_->EnemyShotsEntityList().push_back(entity);

                        y += 125.0f;
                    }


                    if(++num_shots_ >= 6)
                    {
                        num_shots_      = 0;
                        shoot_state_    = SS_PLASMA;
                    }
                }
            }
            break;
        }
    case SS_PLASMA:
        {
            plasma_reload_time_ += time_delta;
            if(plasma_reload_time_ >= 0.1f)
            {
                plasma_reload_time_ = 0.0f;

                int angle_random = 24;
                int half_random_angle = angle_random/2;

                Gfx::IEntityPtr entity(new PlasmaShotEntity(world_));
                PlasmaShotEntity* shot = static_cast<PlasmaShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext(), 2);
                shot->Position(Position() + Math::Vector(0.0f, frame_shooting_offsets_[shoot_frame_], 0.0f));
                shot->Rotation(Math::Vector(0.0f, float((rand()%angle_random)-half_random_angle), 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);

                snd_plasma_->Play2d();

                if(++num_shots_ >= 20)
                {
                    num_shots_      = 0;
                    shoot_state_    = SS_RAIL;
                }
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss2EnemyEntity::Kill()
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
bool MiniBoss2EnemyEntity::TakeDamage(unsigned int damage)
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
