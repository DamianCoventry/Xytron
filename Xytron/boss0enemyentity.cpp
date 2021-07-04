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
#include "boss0enemyentity.h"
#include "iworld.h"
#include "playerentity.h"
#include "basicshotentity.h"
#include "plasmashotentity.h"
#include "nukeexplosionentity.h"
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float ROTATE_THRESHOLD = 0.05f;
}

///////////////////////////////////////////////////////////////////////////////
Boss0EnemyEntity::Boss0EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: BasicEnemyEntity(event_handler, world)
, gun_rotate_time_(0.0f)
, gun_rotate_dir_(false)
, gun_facing_player_(false)
, shoot_rotation_(0.0f)
, shoot_time_(0.0f)
, gun_anim_index_(0)
, plasma_shooting_(false)
, plasma_time_(0.0f)
, plasma_threshold_(0.0f)
, gun_anim_time_(0.0f)
, reloading_(false)
, reload_time_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void Boss0EnemyEntity::BindResources(Util::ResourceContext* resources)
{
    snd_explode_    = resources->FindSound("Sounds/NukeExplosion.wav");
    snd_shoot_      = resources->FindSound("Sounds/CannonShot.wav");
    snd_plasma_     = resources->FindSound("Sounds/PlasmaShot.wav");

    img_shipbot_ = resources->FindImage("Images/Boss0.tga");
    for(unsigned int i = 0; i < 36; i++)
    {
        std::ostringstream oss;
        oss << "Images/Boss0_TopGun" << i << ".tga";
        img_shiptop_[i] = resources->FindImageAnim(oss.str());
    }
}

///////////////////////////////////////////////////////////////////////////////
void Boss0EnemyEntity::Think(float time_delta)
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

    if(gun_facing_player_)
    {
        gun_rotate_time_ += time_delta;
        if(gun_rotate_time_ >= ROTATE_THRESHOLD)
        {
            gun_rotate_time_    = 0.0f;
            gun_facing_player_  = false;
            gun_rotate_dir_     = ClassifyRotationDirection2(world_->Player()->Position());
        }
    }
    else
    {
        if(gun_rotate_dir_)
        {
            shoot_rotation_ += rotate_speed_*time_delta;
            if(shoot_rotation_ >= 360.0f) shoot_rotation_ -= 360.0f;
        }
        else
        {
            shoot_rotation_ -= rotate_speed_*time_delta;
            if(shoot_rotation_ < 0.0f) shoot_rotation_ += 360.0f;
        }

        if(gun_rotate_dir_ != ClassifyRotationDirection2(world_->Player()->Position()))
        {
            gun_facing_player_ = true;
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
void Boss0EnemyEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }

    img_shipbot_->Draw2d(g, Position());

    float adjusted = shoot_rotation_+5.0f;
    if(adjusted >= 360.0f) adjusted -= 360.0f;
    int index = int(adjusted / 10.0f);

    img_shiptop_[index]->CurrentFrame(gun_anim_index_);
    img_shiptop_[index]->Draw2d(g, Position() + Math::Vector(0.0f, -16.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
bool Boss0EnemyEntity::ClassifyRotationDirection2(const Math::Vector& point) const
{
    Math::Vector to_waypoint(point - Position());

    Math::Vector forwards;
    forwards.x_ = -sinf(shoot_rotation_*Math::DTOR);
    forwards.y_ = cosf(shoot_rotation_*Math::DTOR);

    Math::Vector up(0.0f, 0.0f, 1.0f);
    Math::Vector right(forwards.CrossProduct(up));

    float dot = right.DotProduct(to_waypoint);
    return (dot < 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
void Boss0EnemyEntity::ShootThink(float time_delta)
{
    bool play_sound = false;

    if(gun_anim_index_ > 0)
    {
        gun_anim_time_ += time_delta;
        if(gun_anim_time_ >= 0.05f)
        {
            gun_anim_time_ = 0.0f;
            if(++gun_anim_index_ >= 16)
            {
                gun_anim_index_ = 0;
            }
        }
    }

    if(plasma_shooting_)
    {
        plasma_time_ += time_delta;
        if(plasma_time_ >= plasma_threshold_)
        {
            plasma_time_        = 0.0f;
            plasma_shooting_    = false;
        }
        else
        {
            if(reloading_)
            {
                reload_time_ += time_delta;
                if(reload_time_ >= 0.2f)        // 5 shots a second
                {
                    reload_time_ = 0.0f;
                    reloading_ = false;
                }
            }
            else
            {
                snd_plasma_->Play2d();

                Gfx::IEntityPtr entity(new PlasmaShotEntity(world_));
                PlasmaShotEntity* shot = static_cast<PlasmaShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext(), 2);
                shot->Position(Position());
                shot->Rotation(Math::Vector(0.0f, shoot_rotation_, 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);

                reloading_      = true;
                reload_time_    = 0.0f;
                gun_anim_index_ = 1;
            }
        }
        return;
    }

    shoot_time_ += time_delta;
    if(shoot_time_ >= shoot_threshold_)
    {
        shoot_time_         = 0.0f;
        shoot_threshold_    = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;

        if(!world_->Player()->Dead() && world_->Player()->Shootable())
        {
            plasma_time_        = 0.0f;
            plasma_shooting_    = true;
            plasma_threshold_   = shoot_threshold_ / 2.0f;
            reloading_          = false;

            snd_shoot_->Play2d();

            float delta_angle = shoot_rotation_ - 57.5f;
            unsigned int i;

            for(i = 0; i < 4; i++)
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(Position());
                shot->Rotation(Math::Vector(0.0f, delta_angle, 0.0f));
                shot->Speed(shot_speed_);
                world_->EnemyShotsEntityList().push_back(entity);

                delta_angle += 12.0f;
            }

            delta_angle = shoot_rotation_ + 57.5f;
            for(i = 0; i < 4; i++)
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(Position());
                shot->Rotation(Math::Vector(0.0f, delta_angle, 0.0f));
                shot->Speed(shot_speed_);
                world_->EnemyShotsEntityList().push_back(entity);

                delta_angle -= 12.0f;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Boss0EnemyEntity::Kill()
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
bool Boss0EnemyEntity::TakeDamage(unsigned int damage)
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
