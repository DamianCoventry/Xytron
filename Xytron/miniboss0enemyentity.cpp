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
#include "miniboss0enemyentity.h"
#include "iworld.h"
#include "playerentity.h"
#include "basicshotentity.h"
#include "smallenemyexplosionentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float ROTATE_THRESHOLD = 0.25f;
}

///////////////////////////////////////////////////////////////////////////////
MiniBoss0EnemyEntity::MiniBoss0EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: BasicEnemyEntity(event_handler, world)
, rotate_time_(0.0f)
, rotate_dir_(false)
, facing_player_(false)
, rotation_y_(0.0f)
, shoot_time_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss0EnemyEntity::BindResources(Util::ResourceContext* resources)
{
    snd_explosions_.resize(5);
    snd_explosions_[0] = resources->FindSound("Sounds/ShipExplosion0.wav");
    snd_explosions_[1] = resources->FindSound("Sounds/ShipExplosion1.wav");
    snd_explosions_[2] = resources->FindSound("Sounds/ShipExplosion2.wav");
    snd_explosions_[3] = resources->FindSound("Sounds/ShipExplosion3.wav");
    snd_explosions_[4] = resources->FindSound("Sounds/ShipExplosion4.wav");

    snd_shoot_  = resources->FindSound("Sounds/CannonShot.wav");
    img_ship_   = resources->FindImage("Images/MiniBoss0.tga");

    guns_[0].state_             = GS_CLOSED;
    guns_[0].shoot_time_        = 0.0f;
    guns_[0].shooting_          = false;
    guns_[0].shoot_threshold_   = 1.0f + float(rand()%500-250)/1000.0f;
    guns_[0].imgani_            = resources->FindImageAnim("Images/MiniBoss0_TopGunLeft.tga");
    guns_[0].imgani_->CurrentFrame(0);
    guns_[0].offset_            = Math::Vector(-9.0f, 14.0f, 0.0f);
    guns_[0].reload_time_       = 0.0f;

    guns_[1].state_             = GS_CLOSED;
    guns_[1].shoot_time_        = 0.0f;
    guns_[1].shooting_          = false;
    guns_[1].shoot_threshold_   = 1.0f + float(rand()%500-250)/1000.0f;
    guns_[1].imgani_            = resources->FindImageAnim("Images/MiniBoss0_TopGunMiddle.tga");
    guns_[1].imgani_->CurrentFrame(0);
    guns_[1].offset_            = Math::Vector( 2.0f, 14.0f, 0.0f);
    guns_[1].reload_time_       = 0.0f;

    guns_[2].state_             = GS_CLOSED;
    guns_[2].shoot_time_        = 0.0f;
    guns_[2].shooting_          = false;
    guns_[2].shoot_threshold_   = 1.0f + float(rand()%500-250)/1000.0f;
    guns_[2].imgani_            = resources->FindImageAnim("Images/MiniBoss0_TopGunRight.tga");
    guns_[2].imgani_->CurrentFrame(0);
    guns_[2].offset_            = Math::Vector(14.0f, 14.0f, 0.0f);
    guns_[2].reload_time_       = 0.0f;

    guns_[3].state_             = GS_CLOSED;
    guns_[3].shoot_time_        = 0.0f;
    guns_[3].shooting_          = false;
    guns_[3].shoot_threshold_   = 1.0f + float(rand()%500-250)/1000.0f;
    guns_[3].imgani_            = resources->FindImageAnim("Images/MiniBoss0_BotGunLeft.tga");
    guns_[3].imgani_->CurrentFrame(0);
    guns_[3].offset_            = Math::Vector(-10.0f, 32.0f, 0.0f);
    guns_[3].reload_time_       = 0.0f;

    guns_[4].state_             = GS_CLOSED;
    guns_[4].shoot_time_        = 0.0f;
    guns_[4].shooting_          = false;
    guns_[4].shoot_threshold_   = 1.0f + float(rand()%500-250)/1000.0f;
    guns_[4].imgani_            = resources->FindImageAnim("Images/MiniBoss0_BotGunMiddle.tga");
    guns_[4].imgani_->CurrentFrame(0);
    guns_[4].offset_            = Math::Vector(  0.0f, 32.0f, 0.0f);
    guns_[4].reload_time_       = 0.0f;

    guns_[5].state_             = GS_CLOSED;
    guns_[5].shoot_time_        = 0.0f;
    guns_[5].shooting_          = false;
    guns_[5].shoot_threshold_   = 1.0f + float(rand()%500-250)/1000.0f;
    guns_[5].imgani_            = resources->FindImageAnim("Images/MiniBoss0_BotGunRight.tga");
    guns_[5].imgani_->CurrentFrame(0);
    guns_[5].offset_            = Math::Vector(  9.0f, 32.0f, 0.0f);
    guns_[5].reload_time_       = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss0EnemyEntity::Think(float time_delta)
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
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss0EnemyEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }

    img_ship_->Draw2d(g, Position());

    guns_[0].imgani_->Draw2d(g, Position() + guns_[0].offset_);
    guns_[1].imgani_->Draw2d(g, Position() + guns_[1].offset_);
    guns_[2].imgani_->Draw2d(g, Position() + guns_[2].offset_);

    guns_[3].imgani_->Draw2d(g, Position() + guns_[3].offset_);
    guns_[4].imgani_->Draw2d(g, Position() + guns_[4].offset_);
    guns_[5].imgani_->Draw2d(g, Position() + guns_[5].offset_);
}

///////////////////////////////////////////////////////////////////////////////
bool MiniBoss0EnemyEntity::ClassifyRotationDirection2(const Math::Vector& point) const
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
void MiniBoss0EnemyEntity::ShootThink(float time_delta)
{
    bool play_sound = false;

    shoot_time_ += time_delta;
    if(shoot_time_ >= shoot_threshold_)
    {
        shoot_time_ = 0.0f;
        shoot_threshold_ = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;

        if(!world_->Player()->Dead() && world_->Player()->Shootable())
        {
            play_sound = true;

            Gfx::IEntityPtr entity(new BasicShotEntity(world_));
            BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(Position() + Math::Vector(30.0f, 0.0f, 0.0f));
            shot->Rotation(Math::Vector(0.0f, -15.0f, 0.0f));
            shot->Speed(shot_speed_);
            world_->EnemyShotsEntityList().push_back(entity);

            entity.reset(new BasicShotEntity(world_));
            shot = static_cast<BasicShotEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(Position() - Math::Vector(30.0f, 0.0f, 0.0f));
            shot->Rotation(Math::Vector(0.0f, 15.0f, 0.0f));
            shot->Speed(shot_speed_);
            world_->EnemyShotsEntityList().push_back(entity);
        }
    }

    for(unsigned int i = 0; i < 6; i++)
    {
        switch(guns_[i].state_)
        {
        case GS_CLOSED:
            guns_[i].shoot_time_ += time_delta;
            if(guns_[i].shoot_time_ >= guns_[i].shoot_threshold_)
            {
                guns_[i].shoot_time_        = 0.0f;
                guns_[i].shoot_threshold_   = 1.0f + float(rand()%500-250)/1000.0f;
                guns_[i].state_             = GS_OPENING;
            }
            break;
        case GS_OPENING:
            guns_[i].shoot_time_ += time_delta;
            if(guns_[i].shoot_time_ >= 0.1f)
            {
                guns_[i].shoot_time_ = 0.0f;

                guns_[i].imgani_->IncCurrentFrame();
                if(guns_[i].imgani_->CurrentFrame() == 0)
                {
                    guns_[i].imgani_->CurrentFrame(guns_[i].imgani_->ImageCount()-1);
                    guns_[i].state_ = GS_OPENED;
                }
            }
            break;
        case GS_OPENED:
            guns_[i].shoot_time_ += time_delta;
            if(guns_[i].shoot_time_ >= guns_[i].shoot_threshold_)
            {
                guns_[i].reload_time_       = 0.0f;
                guns_[i].shoot_time_        = 0.0f;
                guns_[i].shoot_threshold_   = 1.0f + float(rand()%500-250)/1000.0f;
                guns_[i].state_             = GS_CLOSING;
            }
            else
            {
                guns_[i].reload_time_ += time_delta;
                if(guns_[i].reload_time_ >= 0.33333f)
                {
                    guns_[i].reload_time_   = 0.0f;
                    play_sound              = true;

                    Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                    BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                    shot->BindResources(world_->ResourceContext());
                    shot->Position(Position() + guns_[i].offset_);
                    shot->Rotation(Math::Vector(0.0f, 0.0f, 0.0f));
                    shot->Speed(shot_speed_*1.1f);
                    world_->EnemyShotsEntityList().push_back(entity);
                }
            }
            break;
        case GS_CLOSING:
            guns_[i].shoot_time_ += time_delta;
            if(guns_[i].shoot_time_ >= 0.1f)
            {
                guns_[i].shoot_time_ = 0.0f;

                guns_[i].imgani_->CurrentFrame(guns_[i].imgani_->CurrentFrame() - 1);
                if(guns_[i].imgani_->CurrentFrame() == 0)
                {
                    guns_[i].state_ = GS_CLOSED;
                }
            }
            break;
        }
    }

    if(play_sound)
    {
        snd_shoot_->Play2d();
    }
}

///////////////////////////////////////////////////////////////////////////////
void MiniBoss0EnemyEntity::Kill()
{
    world_->StartScreenFlash(Gfx::Color(1.0f, 1.0f, 1.0f), 3000);

    Gfx::IEntityPtr entity(new SmallEnemyExplosionEntity(Position()));
    SmallEnemyExplosionEntity* explosion = static_cast<SmallEnemyExplosionEntity*>(entity.get());
    explosion->Position(Position());
    explosion->BindResources(world_->ResourceContext());
    world_->ExplosionsEntityList().push_back(entity);

    IEntity::Kill();
}
