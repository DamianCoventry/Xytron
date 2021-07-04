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
#include "basicenemyentity.h"
#include "basicshotentity.h"
#include "playerentity.h"
#include "iworld.h"
#include "smallenemyexplosionentity.h"

///////////////////////////////////////////////////////////////////////////////
BasicEnemyEntity::BasicEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: event_handler_(event_handler)
, world_(world)
, current_way_point_(0)
, state_(STATE_STARTTIMEOUT)
, accel_(75.0f)
, speed_(0.0f)
, max_speed_(200.0f)
, rotate_speed_(200.0f)
, shoot_time_(0.0f)
, shoot_threshold_(0.0f)
, self_destruct_time_(0.0f)
, wait_time_(0.0f)
, wait_threshold_(0.0f)
, shot_speed_(400.0f)
, min_shoot_threshold_(5000)
, max_shoot_threshold_(25000)
, points_(10)
, hit_points_(10)
, waypoint_threshold_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::BindResources(Util::ResourceContext* resources)
{
    imgani_ship_ = resources->FindImageAnim("Images/EnemyType0.tga");

    snd_explosions_.resize(5);
    snd_explosions_[0] = resources->FindSound("Sounds/ShipExplosion0.wav");
    snd_explosions_[1] = resources->FindSound("Sounds/ShipExplosion1.wav");
    snd_explosions_[2] = resources->FindSound("Sounds/ShipExplosion2.wav");
    snd_explosions_[3] = resources->FindSound("Sounds/ShipExplosion3.wav");
    snd_explosions_[4] = resources->FindSound("Sounds/ShipExplosion4.wav");

    snd_shoot_ = resources->FindSound("Sounds/CannonShot.wav");
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::MinShootTimeout(unsigned int min_shoot)
{
    min_shoot_threshold_ = min_shoot;
    shoot_threshold_ = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::MaxShootTimeout(unsigned int max_shoot)
{
    max_shoot_threshold_ = max_shoot; 
    shoot_threshold_ = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::Think(float time_delta)
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
                    state_ = STATE_DECIDING;
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
    }

    if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f))
    {
        ShootThink(time_delta);
    }
    SelfDestructThink(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    switch(state_)
    {
    case STATE_DECIDING:
    case STATE_TURNING:
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
void BasicEnemyEntity::CollideWithPlayer(Gfx::IEntityPtr player)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }
    if(IsColliding(player))
    {
        TakeDamage(10);
        PlayerEntity* temp = static_cast<PlayerEntity*>(player.get());
        temp->TakeDamage(10);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::Draw2d(const Gfx::Graphics& g)
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
bool BasicEnemyEntity::ClassifyRotationDirection(const Math::Vector& point) const
{
    Math::Vector to_waypoint(point - Position());

    Math::Vector forwards;
    forwards.x_ = -sinf(rotation_.y_*Math::DTOR);
    forwards.y_ = cosf(rotation_.y_*Math::DTOR);

    Math::Vector up(0.0f, 0.0f, 1.0f);
    Math::Vector right(forwards.CrossProduct(up));

    float dot = right.DotProduct(to_waypoint);
    return (dot < 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::ShootThink(float time_delta)
{
    shoot_time_ += time_delta;
    if(shoot_time_ >= shoot_threshold_)
    {
        shoot_time_ = 0.0f;
        shoot_threshold_ = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;

        if(!world_->Player()->Dead() && world_->Player()->Shootable())
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

            Gfx::IEntityPtr entity(new BasicShotEntity(world_));
            BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(Position());
            shot->Speed(shot_speed_);
            shot->Rotation(Math::Vector(0.0f, angle, 0.0f));
            world_->EnemyShotsEntityList().push_back(entity);

            snd_shoot_->Play2d();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::SelfDestructThink(float time_delta)
{
    if(current_way_point_ == int(way_points_.size())-1 || way_points_.empty())
    {
        self_destruct_time_ += time_delta;
        if(self_destruct_time_ >= 5.0f)     // Entity has 5 seconds to reach last waypoint
        {
            Kill();
            event_handler_->OnEnemyExpired(shared_from_this());
            OutputDebugString("A BasicEnemyEntity self destructed\n");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::Kill()
{
    Gfx::IEntityPtr entity(new SmallEnemyExplosionEntity(Position()));
    SmallEnemyExplosionEntity* explosion = static_cast<SmallEnemyExplosionEntity*>(entity.get());
    explosion->Position(Position());
    explosion->BindResources(world_->ResourceContext());
    world_->ExplosionsEntityList().push_back(entity);

    IEntity::Kill();
}

///////////////////////////////////////////////////////////////////////////////
void BasicEnemyEntity::OnFree()
{
    event_handler_->OnEnemyFreed(shared_from_this());
}

///////////////////////////////////////////////////////////////////////////////
bool BasicEnemyEntity::TakeDamage(unsigned int damage)
{
    if(damage >= hit_points_)
    {
        hit_points_ = 0;
        snd_explosions_[rand()%5]->Play2d();
        Kill();
        event_handler_->OnEnemyDestroyed(shared_from_this());
        return true;
    }
    hit_points_ -= damage;
    return false;
}
