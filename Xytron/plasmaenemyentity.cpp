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
#include "plasmaenemyentity.h"
#include "iworld.h"
#include "plasmashotentity.h"
#include "playerentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float ROTATE_THRESHOLD = 0.25f;
}

///////////////////////////////////////////////////////////////////////////////
PlasmaEnemyEntity::PlasmaEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: BasicEnemyEntity(event_handler, world)
, sit_time_(0.0f)
, sit_threshold_(0.0f)
, min_sit_timeout_(4000)
, max_sit_timeout_(8000)
, shoot_state_(SS_WAITING)
, plasma_reload_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaEnemyEntity::BindResources(Util::ResourceContext* resources)
{
    imgani_ship_ = resources->FindImageAnim("Images/EnemyType4.tga");

    snd_explosions_.resize(5);
    snd_explosions_[0] = resources->FindSound("Sounds/ShipExplosion0.wav");
    snd_explosions_[1] = resources->FindSound("Sounds/ShipExplosion1.wav");
    snd_explosions_[2] = resources->FindSound("Sounds/ShipExplosion2.wav");
    snd_explosions_[3] = resources->FindSound("Sounds/ShipExplosion3.wav");
    snd_explosions_[4] = resources->FindSound("Sounds/ShipExplosion4.wav");

    snd_shoot_ = resources->FindSound("Sounds/PlasmaShot.wav");
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaEnemyEntity::Think(float time_delta)
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
                    Kill();
                    event_handler_->OnEnemyExpired(shared_from_this());
                    OutputDebugString("A BasicEnemyEntity reached its last waypoint\n");
                }
                else
                {
                    sit_time_       = 0.0f;
                    sit_threshold_  = float(min_sit_timeout_ + (rand()%(max_sit_timeout_ - min_sit_timeout_)))/1000.0f;
                    state_          = STATE_SITTING;
                    rotate_time_    = 0.0f;
                    rotate_dir_     = ClassifyRotationDirection(world_->Player()->Position());
                    facing_player_  = false;
                }
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

            ShootThink(time_delta);
        }
        break;
    }

    SelfDestructThink(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaEnemyEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    switch(state_)
    {
    case STATE_DECIDING:
    case STATE_TURNING:
    case STATE_SITTING:
        speed_ -= accel_*time_delta;
        if(speed_ < 0.0f)
        {
            speed_ = 0.0f;
        }
        break;
    case STATE_MOVING:
        speed_ += accel_*time_delta;
        if(speed_ > max_speed_)
        {
            speed_ = max_speed_;
        }
        break;
    }

    Math::Vector pos = Position();
    pos.x_ += (speed_*time_delta) * -sinf(rotation_.y_*Math::DTOR);
    pos.y_ += (speed_*time_delta) * cosf(rotation_.y_*Math::DTOR);
    Position(pos);
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaEnemyEntity::ShootThink(float time_delta)
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
            plasma_reload_      = 0.0f;
        }
        break;
    case SS_SHOOTING:
        if(!world_->Player()->Dead() && world_->Player()->Shootable())
        {
            FireShot(time_delta);
        }
        shoot_time_ += time_delta;
        if(shoot_time_ >= shoot_threshold_)
        {
            shoot_time_         = 0.0f;
            shoot_threshold_    = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;
            shoot_state_        = SS_WAITING;
        }
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaEnemyEntity::FireShot(float time_delta)
{
    plasma_reload_ += time_delta;
    if(plasma_reload_ >= 0.3333f)       // 3 shots a second
    {
        plasma_reload_ = 0.0f;

        // Work out the angle to the player
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

        // Right, now we've got the angle.  Let's randomize it a bit.
        int angle_random = 24;
        int half_random_angle = angle_random/2;

        Gfx::IEntityPtr entity(new PlasmaShotEntity(world_));
        PlasmaShotEntity* shot = static_cast<PlasmaShotEntity*>(entity.get());
        shot->BindResources(world_->ResourceContext(), 0);
        shot->Position(Position());
        shot->Rotation(Math::Vector(0.0f, angle+float((rand()%angle_random)-half_random_angle), 0.0f));
        world_->EnemyShotsEntityList().push_back(entity);

        snd_shoot_->Play2d();
    }
}