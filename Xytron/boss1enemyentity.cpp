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
#include "boss1enemyentity.h"
#include "iworld.h"
#include "playerentity.h"
#include "basicshotentity.h"
#include "nukeexplosionentity.h"
#include "missileshotentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float ROTATE_THRESHOLD = 0.25f;

    float missile_x_offsets_[8] = { -61.0f, -45.0f, -28.0f, -10.0f, 10.0f, 28.0f, 45.0f, 61.0f };
    float missile_y_offsets_[8] = { 30.0f, 37.0f, 42.0f, 42.0f, 42.0f, 42.0f, 37.0f, 30.0f };

    Math::Vector cannon_offsets_[10] =
    {
        Math::Vector(-69.0f,   7.0f, 0.0f),
        Math::Vector(-47.0f, -11.0f, 0.0f),
        Math::Vector(-45.0f,  21.0f, 0.0f),
        Math::Vector(-24.0f,  18.0f, 0.0f),
        Math::Vector(-17.0f, -21.0f, 0.0f),
        Math::Vector( 17.0f, -21.0f, 0.0f),
        Math::Vector( 24.0f,  18.0f, 0.0f),
        Math::Vector( 45.0f,  21.0f, 0.0f),
        Math::Vector( 47.0f, -11.0f, 0.0f),
        Math::Vector( 69.0f,   7.0f, 0.0f)
    };

    float angle_adjustments_[10] =
    {
        45, 35, 25, 15, 5, -5, -15, -25, -35, -45
    };
}

///////////////////////////////////////////////////////////////////////////////
Boss1EnemyEntity::Boss1EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: BasicEnemyEntity(event_handler, world)
, rotate_time_(0.0f)
, rotate_dir_(false)
, facing_player_(false)
, rotation_y_(0.0f)
, shoot_state_(SS_CANNON_PAUSE)
, ss_time_(0.0f)
, missile_index_(0)
, imgani_current_(0)
, shots_remaining_(10)
, alternate_(false)
, end_gun_angle_time_(0.0f)
, gun_time_(0.0f)
, end_gun_state_(EGS_TURNING)
, left_gun_rotate_time_(0.0f)
, right_gun_rotate_time_(0.0f)
, left_gun_angle_(0.0f)
, right_gun_angle_(0.0f)
{
    left_gun_dest_angle_        = float(rand()%90);
    right_gun_dest_angle_       = float(rand()%90);
    ss_threshold_               = 1.0f + (float(250+(rand()%500)) / 1000.0f);
    end_gun_angle_threshold_    = 1.0f + (float(250+(rand()%500)) / 1000.0f);
}

///////////////////////////////////////////////////////////////////////////////
void Boss1EnemyEntity::BindResources(Util::ResourceContext* resources)
{
    snd_explode_    = resources->FindSound("Sounds/NukeExplosion.wav");
    snd_shoot_      = resources->FindSound("Sounds/CannonShot.wav");
    snd_missile0_   = resources->FindSound("Sounds/MissilesShot0.wav");
    snd_missile1_   = resources->FindSound("Sounds/MissilesShot1.wav");
    imgani_ship_    = resources->FindImageAnim("Images/Boss1_FlipGun.tga");

    imgani_endgunleft_  = resources->FindImageAnim("Images/Boss1_EndGunLeft.tga");
    imgani_endgunright_ = resources->FindImageAnim("Images/Boss1_EndGunRight.tga");
}

///////////////////////////////////////////////////////////////////////////////
void Boss1EnemyEntity::Think(float time_delta)
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

    if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f) && !world_->Player()->Dead() && world_->Player()->Shootable())
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
}

///////////////////////////////////////////////////////////////////////////////
void Boss1EnemyEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }

    imgani_ship_->CurrentFrame(imgani_current_);
    imgani_ship_->Draw2d(g, Position());

    imgani_endgunleft_->CurrentFrame(int(left_gun_angle_/11.25f));
    imgani_endgunleft_->Draw2d(g, Position() + Math::Vector(-81.0f, 0.0f, 0.0f));

    imgani_endgunright_->CurrentFrame(int(right_gun_angle_/11.25f));
    imgani_endgunright_->Draw2d(g, Position() + Math::Vector(81.0f, 0.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
bool Boss1EnemyEntity::ClassifyRotationDirection2(const Math::Vector& point) const
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
void Boss1EnemyEntity::ShootThink(float time_delta)
{
    switch(shoot_state_)
    {
    case SS_MISSILES:
        {
            ss_time_ += time_delta;
            if(ss_time_ >= 0.1f)
            {
                ss_time_ = 0.0f;

                rand()%2 == 0 ? snd_missile0_->Play2d() : snd_missile1_->Play2d();

                Gfx::IEntityPtr entity(new MissileShotEntity(world_, false));
                MissileShotEntity* shot = static_cast<MissileShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(Position() + Math::Vector(missile_x_offsets_[4-missile_index_], missile_y_offsets_[4-missile_index_], 0.0f));
                shot->Rotation(Math::Vector());
                world_->EnemyShotsEntityList().push_back(entity);

                entity.reset(new MissileShotEntity(world_, false));
                shot = static_cast<MissileShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(Position() + Math::Vector(missile_x_offsets_[5+missile_index_], missile_y_offsets_[4-missile_index_], 0.0f));
                shot->Rotation(Math::Vector());
                world_->EnemyShotsEntityList().push_back(entity);

                if(++missile_index_ >= 5)
                {
                    missile_index_  = 0;
                    shoot_state_    = SS_MISSILE_PAUSE;
                }
            }

            break;
        }
    case SS_MISSILE_PAUSE:
        ss_time_ += time_delta;
        if(ss_time_ >= ss_threshold_)
        {
            ss_time_        = 0.0f;
            ss_threshold_   = 1.0f + (float(250+(rand()%500)) / 1000.0f);
            shoot_state_    = SS_CANNON_OPENING;
        }
        break;
    case SS_CANNON_OPENING:
        {
            ss_time_ += time_delta;
            if(ss_time_ >= 0.05f)
            {
                ss_time_ = 0.0f;
                if(++imgani_current_ >= 16)
                {
                    imgani_current_ = 15;
                    shoot_state_    = SS_CANNON_SHOOTING;

                    FireShot();
                }
            }
            break;
        }
    case SS_CANNON_SHOOTING:
        ss_time_ += time_delta;
        if(ss_time_ >= 0.8f)
        {
            ss_time_ = 0.0f;

            if(shots_remaining_ > 1)
            {
                FireShot();
                shots_remaining_--;
            }
            else
            {
                shots_remaining_ = 10;

                ss_time_        = 0.0f;
                ss_threshold_   = 1.0f + (float(250+(rand()%500)) / 1000.0f);
                shoot_state_    = SS_CANNON_CLOSING;
            }
        }
        break;
    case SS_CANNON_CLOSING:
        ss_time_ += time_delta;
        if(ss_time_ >= 0.05f)
        {
            ss_time_ = 0.0f;
            if(--imgani_current_ < 0)
            {
                imgani_current_ = 0;
                shoot_state_    = SS_CANNON_PAUSE;
            }
        }
        break;
    case SS_CANNON_PAUSE:
        ss_time_ += time_delta;
        if(ss_time_ >= ss_threshold_)
        {
            ss_time_        = 0.0f;
            ss_threshold_   = 1.0f + (float(250+(rand()%500)) / 1000.0f);
            shoot_state_    = SS_MISSILES;
        }
        break;
    }

    switch(end_gun_state_)
    {
    case EGS_TURNING:
        left_gun_rotate_time_ += time_delta;
        if(left_gun_rotate_time_ >= 0.05f)
        {
            left_gun_rotate_time_ = 0.0f;
            if(left_gun_angle_ < left_gun_dest_angle_)
            {
                left_gun_angle_ += 1.0f;
            }
            else if(left_gun_angle_ > left_gun_dest_angle_)
            {
                left_gun_angle_ -= 1.0f;
            }
        }
        right_gun_rotate_time_ += time_delta;
        if(right_gun_rotate_time_ >= 0.05f)
        {
            right_gun_rotate_time_ = 0.0f;
            if(right_gun_angle_ < right_gun_dest_angle_)
            {
                right_gun_angle_ += 1.0f;
            }
            else if(right_gun_angle_ > right_gun_dest_angle_)
            {
                right_gun_angle_ -= 1.0f;
            }
        }
        if(left_gun_angle_ == left_gun_dest_angle_ && right_gun_angle_ == right_gun_dest_angle_)
        {
            end_gun_state_ = EGS_SHOOTING;
        }
        break;
    case EGS_SHOOTING:
        end_gun_angle_time_ += time_delta;
        if(end_gun_angle_time_ >= end_gun_angle_threshold_)
        {
            end_gun_angle_time_         = 0.0f;
            end_gun_angle_threshold_    = 1.0f + (float(250+(rand()%500)) / 1000.0f);

            left_gun_dest_angle_    = float(rand()%90);
            right_gun_dest_angle_   = float(rand()%90);
            end_gun_state_          = EGS_TURNING;
        }

        gun_time_ += time_delta;
        if(gun_time_ >= 0.5f)
        {
            gun_time_ = 0.0f;

            Math::Vector pos(Position() + Math::Vector(-68.0f, -8.0f, 0.0f));   // Translate to the end of the ship
            pos += Math::Vector(                                                // Translate to the end of the gun's barrell
                25.0f * -sinf(left_gun_angle_*Math::DTOR),
                25.0f * cosf(left_gun_angle_*Math::DTOR),
                0.0f);

            Gfx::IEntityPtr entity(new BasicShotEntity(world_));
            BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(pos);
            shot->Speed(shot_speed_);
            shot->Rotation(Math::Vector(0.0f, left_gun_angle_, 0.0f));
            world_->EnemyShotsEntityList().push_back(entity);

            pos = Position() + Math::Vector(68.0f, -8.0f, 0.0f);                // Translate to the end of the ship
            pos += Math::Vector(                                                // Translate to the end of the gun's barrell
                25.0f * sinf(right_gun_angle_*Math::DTOR),
                25.0f * cosf(right_gun_angle_*Math::DTOR),
                0.0f);

            entity.reset(new BasicShotEntity(world_));
            shot = static_cast<BasicShotEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(pos);
            shot->Speed(shot_speed_);
            shot->Rotation(Math::Vector(0.0f, 360.0f-right_gun_angle_, 0.0f));
            world_->EnemyShotsEntityList().push_back(entity);
        }
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Boss1EnemyEntity::Kill()
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
bool Boss1EnemyEntity::TakeDamage(unsigned int damage)
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
void Boss1EnemyEntity::FireShot()
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

    for(unsigned int i = 0; i < 10; i++)
    {
        Gfx::IEntityPtr entity(new BasicShotEntity(world_));
        BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
        shot->BindResources(world_->ResourceContext());
        shot->Position(Position() + cannon_offsets_[i]);
        shot->Speed(shot_speed_);
        shot->Rotation(Math::Vector(0.0f, angle + angle_adjustments_[alternate_ ? rand()%10 : i], 0.0f));
        world_->EnemyShotsEntityList().push_back(entity);
    }

    snd_shoot_->Play2d();
    alternate_ = !alternate_;
}
