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
#include "cannonweapon.h"
#include "iworld.h"
#include "basicshotentity.h"
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////
CannonWeapon::CannonWeapon(IWorld* world, Gfx::IEntity* owner)
: IWeapon(PickupEntity::PICKUP_GUN_CANNON, world, owner)
, reload_time_(0)
, upgrade_(0)
, name_("Cannon")
{
}

///////////////////////////////////////////////////////////////////////////////
void CannonWeapon::BindResources(Util::ResourceContext* resources)
{
    snd_shoot_ = resources->FindSound("Sounds/CannonShot.wav");
}

///////////////////////////////////////////////////////////////////////////////
bool CannonWeapon::Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    unsigned long now = timeGetTime();
    if(now - reload_time_ >= 200)       // 5 shots a second
    {
        reload_time_ = now;
        shot_positions->clear();

        snd_shoot_->Play2d();

        switch(upgrade_)
        {
        case 0:         // Upgrade level 0.  The default, a single vertical shot.
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position);
                shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                shot_positions->push_back(position);
                break;
            }
        case 1:         // Upgrade level 1.  Two shots, moving in a slight 'V'
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position + Math::Vector(10.0f, 0.0f, 0.0f));
                shot->Rotation(Math::Vector(0.0f, rotation+5.0f, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                entity.reset(new BasicShotEntity(world_));
                shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position - Math::Vector(10.0f, 0.0f, 0.0f));
                shot->Rotation(Math::Vector(0.0f, rotation-5.0f, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                shot_positions->push_back(position + Math::Vector(10.0f, 0.0f, 0.0f));
                shot_positions->push_back(position - Math::Vector(10.0f, 0.0f, 0.0f));
                break;
            }
        case 2:         // Upgrade level 2.  Three shots, one vertical, the other 2 in a slight 'V'
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position);
                shot->Rotation(Math::Vector(0.0f, rotation, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                entity.reset(new BasicShotEntity(world_));
                shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position + Math::Vector(10.0f, 10.0f, 0.0f));
                shot->Rotation(Math::Vector(0.0f, rotation+7.5f, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                entity.reset(new BasicShotEntity(world_));
                shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position + Math::Vector(-10.0f, 10.0f, 0.0f));
                shot->Rotation(Math::Vector(0.0f, rotation-7.5f, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                shot_positions->push_back(position);
                shot_positions->push_back(position + Math::Vector(10.0f, 10.0f, 0.0f));
                shot_positions->push_back(position + Math::Vector(-10.0f, 10.0f, 0.0f));
                break;
            }
        case 3:         // Upgrade level 3.  Four shots, all moving at an angle upwards
            {
                Gfx::IEntityPtr entity(new BasicShotEntity(world_));
                BasicShotEntity* shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position + Math::Vector(10.0f, 0.0f, 0.0f));
                shot->Rotation(Math::Vector(0.0f, rotation+5.0f, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                entity.reset(new BasicShotEntity(world_));
                shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position - Math::Vector(10.0f, 0.0f, 0.0f));
                shot->Rotation(Math::Vector(0.0f, rotation-5.0f, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                entity.reset(new BasicShotEntity(world_));
                shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position + Math::Vector(20.0f, 10.0f, 0.0f));
                shot->Rotation(Math::Vector(0.0f, rotation+15.0f, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                entity.reset(new BasicShotEntity(world_));
                shot = static_cast<BasicShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext());
                shot->Position(position + Math::Vector(-20.0f, 10.0f, 0.0f));
                shot->Rotation(Math::Vector(0.0f, rotation-15.0f, 0.0f));
                world_->PlayerShotsEntityList().push_back(entity);

                shot_positions->push_back(position + Math::Vector(10.0f, 0.0f, 0.0f));
                shot_positions->push_back(position - Math::Vector(10.0f, 0.0f, 0.0f));
                shot_positions->push_back(position + Math::Vector(20.0f, 10.0f, 0.0f));
                shot_positions->push_back(position + Math::Vector(-20.0f, 10.0f, 0.0f));
                break;
            }
        }
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool CannonWeapon::IncUpgrade()
{
    // return true if upgraded, false if already at max upgrade
    if(upgrade_ == 3)
    {
        return false;
    }
    ++upgrade_;
    return true;
}
