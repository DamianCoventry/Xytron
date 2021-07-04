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
#include "boss2enemyentity.h"
#include "statewave.h"
#include "iworld.h"
#include "playerentity.h"
#include "basicshotentity.h"
#include "railshotentity.h"
#include "plasmashotentity.h"
#include "nukeexplosionentity.h"
#include "railenemyentity.h"
#include "rocketenemyentity.h"
#include "spawnenemysparksentity.h"

#include <sstream>


///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float ROTATE_THRESHOLD = 0.05f;

    Math::Vector gun_offsets_[8] =
    {
        Math::Vector(-27.0f, 53.0f, 0.0f),
        Math::Vector(-23.0f, 61.0f, 0.0f),
        Math::Vector(-16.0f, 67.0f, 0.0f),
        Math::Vector( -6.0f, 71.0f, 0.0f),
        Math::Vector(  4.0f, 71.0f, 0.0f),
        Math::Vector( 14.0f, 67.0f, 0.0f),
        Math::Vector( 22.0f, 62.0f, 0.0f),
        Math::Vector( 26.0f, 53.0f, 0.0f)
    };

    float gun_angles_[8] = {  50.0f,  35.0f,  20.0f,   5.0f,
                              -5.0f, -20.0f, -35.0f, -50.0f };
}

///////////////////////////////////////////////////////////////////////////////
Boss2EnemyEntity::Boss2EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: BasicEnemyEntity(event_handler, world)
, door_frame_(0)
, cannon_time_(0.0f)
, rail_time_(0.0f)
, emit_time_(0.0f)
, plasma_time_(0.0f)
, plasma_count_(0)
, emit_state_(ES_CLOSED)
{
    cannon_threshold_   = 2.0f + (float(rand()%500 - 250) / 1000.0f);
    rail_threshold_     = 1.5f + (float(rand()%500 - 250) / 1000.0f);
    emit_threshold_     = 5.0f + (float(rand()%1000 - 500) / 1000.0f);
}

///////////////////////////////////////////////////////////////////////////////
void Boss2EnemyEntity::BindResources(Util::ResourceContext* resources)
{
    snd_explode_    = resources->FindSound("Sounds/NukeExplosion.wav");
    snd_shoot_      = resources->FindSound("Sounds/CannonShot.wav");
    snd_rail_       = resources->FindSound("Sounds/RailShot.wav");
    snd_plasma_     = resources->FindSound("Sounds/PlasmaShot.wav");
    snd_teleport_   = resources->FindSound("Sounds/Teleport.wav");
    imgani_ship_    = resources->FindImageAnim("Images/Boss2.tga");
}

///////////////////////////////////////////////////////////////////////////////
void Boss2EnemyEntity::Think(float time_delta)
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

    ShootThink(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void Boss2EnemyEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }

    imgani_ship_->CurrentFrame(door_frame_);
    imgani_ship_->Draw2d(g, Position());
}

///////////////////////////////////////////////////////////////////////////////
void Boss2EnemyEntity::ShootThink(float time_delta)
{
    if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f) && !world_->Player()->Dead() && world_->Player()->Shootable())
    {
        cannon_time_ += time_delta;
        if(cannon_time_ >= cannon_threshold_)
        {
            cannon_time_        = 0.0f;
            cannon_threshold_   = 2.0f + (float(rand()%500 - 250) / 1000.0f);

            for(unsigned int i = 0; i < 8; i++)
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(Position() + gun_offsets_[i]);
                shot->Speed(shot_speed_);
                shot->Rotation(Math::Vector(0.0f, gun_angles_[i], 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);
            }

            plasma_count_ = 8;
            snd_shoot_->Play2d();
        }

        if(plasma_count_ > 0)
        {
            plasma_time_ += time_delta;
            if(plasma_time_ >= 0.05f)
            {
                plasma_time_ = 0.0f;
                plasma_count_--;
                snd_plasma_->Play2d();

                Gfx::IEntityPtr entity(new PlasmaShotEntity(world_));
                PlasmaShotEntity* shot = static_cast<PlasmaShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext(), 3);
                shot->Position(Position() + gun_offsets_[plasma_count_]);
                shot->Rotation(Math::Vector(0.0f, gun_angles_[plasma_count_], 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);
            }
        }

        rail_time_ += time_delta;
        if(rail_time_ >= rail_threshold_)
        {
            rail_time_        = 0.0f;
            rail_threshold_   = 1.5f + (float(rand()%500 - 250) / 1000.0f);

            float y = Position().y_ + 137.5f;
            while(y < 675.0f)
            {
                Gfx::IEntityPtr entity(new RailShotEntity(world_));
                RailShotEntity* shot = static_cast<RailShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext(), 3);
                shot->Damage(10);
                shot->Position(Math::Vector(Position().x_, y, 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);

                y += 125.0f;
            }

            snd_rail_->Play2d();
        }
    }

    switch(emit_state_)
    {
    case ES_CLOSED:
        if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f) && !world_->Player()->Dead() && world_->Player()->Shootable())
        {
            if(world_->NumEnemyEntities() < 8)
            {
                emit_time_ += time_delta;
                if(emit_time_ >= emit_threshold_)
                {
                    emit_time_      = 0.0f;
                    emit_threshold_ = 1.0f + (float(rand()%1000 - 500) / 1000.0f);
                    emit_state_     = ES_OPENING;
                }
            }
        }
        break;
    case ES_OPENING:
        emit_time_ += time_delta;
        if(emit_time_ >= 0.1f)
        {
            emit_time_ = 0.0f;
            if(++door_frame_ >= 20)
            {
                door_frame_ = 15;
                emit_state_ = ES_OPENED;
                SpawnEntities();
            }
        }
        break;
    case ES_OPENED:
        emit_time_ += time_delta;
        if(emit_time_ >= emit_threshold_)
        {
            emit_time_      = 0.0f;
            emit_threshold_ = 5.0f + (float(rand()%1000 - 500) / 1000.0f);
            emit_state_     = ES_CLOSING;
        }
        break;
    case ES_CLOSING:
        emit_time_ += time_delta;
        if(emit_time_ >= 0.1f)
        {
            emit_time_ = 0.0f;
            if(--door_frame_ < 0)
            {
                door_frame_ = 0;
                emit_state_ = ES_CLOSED;
            }
        }
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Boss2EnemyEntity::Kill()
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
bool Boss2EnemyEntity::TakeDamage(unsigned int damage)
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
void Boss2EnemyEntity::SpawnEntities()
{
    StateWave* state = static_cast<StateWave*>(world_->CurrentState());

    Gfx::IEntityPtr entity(new RailEnemyEntity(state, world_));
    RailEnemyEntity* rail_enemy = static_cast<RailEnemyEntity*>(entity.get());

    rail_enemy->BindResources(world_->ResourceContext());
    rail_enemy->StartTimeout(0.0f);
    rail_enemy->MinShootTimeout(2500);
    rail_enemy->MaxShootTimeout(3500);
    rail_enemy->Position(Position() - Math::Vector(32.0f, 0.0f, 0.0f));
    rail_enemy->Points(20);
    rail_enemy->ShotSpeed(shot_speed_);
    rail_enemy->MaxSpeed(250);
    rail_enemy->HitPoints(20);
    rail_enemy->WayPoints(GenerateTopOnScreenWaypoints(50));
    rail_enemy->SetMovementSpeed(200.0f, 90.0f);

    world_->EnemyEntityList().push_back(entity);

    entity.reset(new RocketEnemyEntity(state, world_));
    RocketEnemyEntity* rocket_enemy = static_cast<RocketEnemyEntity*>(entity.get());

    rocket_enemy->BindResources(world_->ResourceContext());
    rocket_enemy->StartTimeout(0.0f);
    rocket_enemy->MinShootTimeout(2500);
    rocket_enemy->MaxShootTimeout(3500);
    rocket_enemy->Position(Position() + Math::Vector(32.0f, 0.0f, 0.0f));
    rocket_enemy->Points(20);
    rocket_enemy->ShotSpeed(shot_speed_);
    rocket_enemy->MaxSpeed(250);
    rocket_enemy->HitPoints(20);
    rocket_enemy->WayPoints(GenerateTopOnScreenWaypoints(50));
    rocket_enemy->SetMovementSpeed(200.0f, 270.0f);

    world_->EnemyEntityList().push_back(entity);



    Math::Vector pos(Position() + Math::Vector(32.0f, 0.0f, 0.0f));
    entity.reset(new SpawnEnemySparksEntity(pos, 270.0f, 120, 50));
    SpawnEnemySparksEntity* sparks = static_cast<SpawnEnemySparksEntity*>(entity.get());
    sparks->BindResources(world_->ResourceContext());
    sparks->Position(pos);

    world_->ExplosionsEntityList().push_back(entity);

    pos = Position() - Math::Vector(32.0f, 0.0f, 0.0f);
    entity.reset(new SpawnEnemySparksEntity(pos, 90.0f, 120, 50));
    sparks = static_cast<SpawnEnemySparksEntity*>(entity.get());
    sparks->BindResources(world_->ResourceContext());
    sparks->Position(pos);

    world_->ExplosionsEntityList().push_back(entity);

    snd_teleport_->Play2d();
}

///////////////////////////////////////////////////////////////////////////////
Math::Vector Boss2EnemyEntity::ChooseTopOnscreenPoint() const
{
    float x = float(rand()%300);
    float y = float(rand()%300);

    if(rand()%2 == 0)
    {
        return Math::Vector(100.0f+x, y, 0.0f);
    }
    return Math::Vector(x+400, y, 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
Math::VectorList Boss2EnemyEntity::GenerateTopOnScreenWaypoints(unsigned int num_waypoints) const
{
    Math::VectorList way_points;
    for(unsigned int i = 0; i < num_waypoints; i++)
    {
        way_points.push_back(ChooseTopOnscreenPoint());
    }
    way_points.push_back(ChooseTopOffscreenPoint());
    return way_points;
}

///////////////////////////////////////////////////////////////////////////////
Math::Vector Boss2EnemyEntity::ChooseTopOffscreenPoint() const
{
    return Math::Vector(100.0f+float(rand()%600), -100.0f, 0.0f);
}
