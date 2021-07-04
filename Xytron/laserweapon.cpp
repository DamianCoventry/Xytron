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
#include "laserweapon.h"
#include "iworld.h"
#include "lasershotsmallentity.h"
#include "lasershotlargeentity.h"
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////
LaserWeapon::LaserWeapon(IWorld* world, Gfx::IEntity* owner)
: IWeapon(PickupEntity::PICKUP_GUN_LASER, world, owner)
, reload_time_(0)
, upgrade_(0)
, next_shot_time_(0.0f)
, shot_count_(0)
, state_(STATE_IDLE)
, name_("Laser")
{
}

///////////////////////////////////////////////////////////////////////////////
void LaserWeapon::BindResources(Util::ResourceContext* resources)
{
    snd_shoot_ = resources->FindSound("Sounds/LaserShot.wav");
}

///////////////////////////////////////////////////////////////////////////////
bool LaserWeapon::Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    if(state_ == STATE_IDLE)
    {
        unsigned long now = timeGetTime();
        if(now - reload_time_ >= 250)       // 4 shots a second
        {
            shot_positions->clear();

            reload_time_    = now;
            next_shot_time_ = 0.0f;
            shot_count_     = 0;
            state_          = STATE_SHOOTING;

            rotation_ = rotation;
            FireShot(position, rotation_, shot_positions);
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool LaserWeapon::IncUpgrade()
{
    // return true if upgraded, false if already at max upgrade
    if(upgrade_ == 3)
    {
        return false;
    }
    ++upgrade_;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void LaserWeapon::Think(float time_delta)
{
    if(state_ == STATE_SHOOTING)
    {
        next_shot_time_ += time_delta;
        if(next_shot_time_ >= 0.04f)
        {
            next_shot_time_ = 0.0f;
            FireShot(owner_->Position(), rotation_, NULL);

            if(++shot_count_ >= 3)
            {
                shot_count_ = 0;
                state_      = STATE_IDLE;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void LaserWeapon::FireShot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    snd_shoot_->Play2d();

    switch(upgrade_)
    {
    case 0:         // Upgrade level 0.  The default, one vertical moving small shot.
        {
            Gfx::IEntityPtr entity(new LaserShotSmallEntity(world_));
            LaserShotSmallEntity* shot = static_cast<LaserShotSmallEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(position);
            shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            if(shot_positions)
            {
                shot_positions->push_back(position);
            }
            break;
        }
    case 1:         // Upgrade level 1.  Two vertical moving small shots
        {
            Gfx::IEntityPtr entity(new LaserShotSmallEntity(world_));
            LaserShotSmallEntity* shot = static_cast<LaserShotSmallEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(position + Math::Vector(10.0f, 0.0f, 0.0f));
            shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            entity.reset(new LaserShotSmallEntity(world_));
            shot = static_cast<LaserShotSmallEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(position - Math::Vector(10.0f, 0.0f, 0.0f));
            shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            if(shot_positions)
            {
                shot_positions->push_back(position + Math::Vector(10.0f, 0.0f, 0.0f));
                shot_positions->push_back(position - Math::Vector(10.0f, 0.0f, 0.0f));
            }
            break;
        }
    case 2:         // Upgrade level 2.  Three vertical moving small shots
        {
            Gfx::IEntityPtr entity(new LaserShotSmallEntity(world_));
            LaserShotSmallEntity* shot = static_cast<LaserShotSmallEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(position);
            shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            entity.reset(new LaserShotSmallEntity(world_));
            shot = static_cast<LaserShotSmallEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(position + Math::Vector(10.0f, 0.0f, 0.0f));
            shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            entity.reset(new LaserShotSmallEntity(world_));
            shot = static_cast<LaserShotSmallEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(position + Math::Vector(-10.0f, 0.0f, 0.0f));
            shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            if(shot_positions)
            {
                shot_positions->push_back(position);
                shot_positions->push_back(position + Math::Vector(10.0f, 0.0f, 0.0f));
                shot_positions->push_back(position + Math::Vector(-10.0f, 0.0f, 0.0f));
            }
            break;
        }
    case 3:         // Upgrade level 3.  One large vertical moving shot.
        {
            Gfx::IEntityPtr entity(new LaserShotLargeEntity(world_));
            LaserShotLargeEntity* shot = static_cast<LaserShotLargeEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(position);
            shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            if(shot_positions)
            {
                shot_positions->push_back(position);
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void LaserWeapon::Reset()
{
    reload_time_    = 0;
    next_shot_time_ = 0.0f;
    shot_count_     = 0;
    state_          = STATE_IDLE;
}
