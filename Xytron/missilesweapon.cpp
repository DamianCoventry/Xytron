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
#include "missilesweapon.h"
#include "iworld.h"
#include "missileshotentity.h"
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////
MissilesWeapon::MissilesWeapon(IWorld* world, Gfx::IEntity* owner)
: IWeapon(PickupEntity::PICKUP_GUN_MISSILES, world, owner)
, reload_time_(0)
, upgrade_(0)
, name_("Missiles")
{
}

///////////////////////////////////////////////////////////////////////////////
void MissilesWeapon::BindResources(Util::ResourceContext* resources)
{
    snd_shoot0_ = resources->FindSound("Sounds/MissilesShot0.wav");
    snd_shoot1_ = resources->FindSound("Sounds/MissilesShot1.wav");
}

///////////////////////////////////////////////////////////////////////////////
bool MissilesWeapon::Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    unsigned long now = timeGetTime();
    if(now - reload_time_ >= 333)       // 3 shots a second
    {
        reload_time_ = now;
        shot_positions->clear();

        (rand()%2 == 0) ? snd_shoot0_->Play2d() : snd_shoot1_->Play2d();

        unsigned int num_missiles = upgrade_ + 2;
        int angle;
        switch(upgrade_)
        {
        case 0: angle = 10; break;
        case 1: angle = 17; break;
        case 2: angle = 23; break;
        case 3: angle = 30; break;
        }
        int half_angle = angle/2;

        for(unsigned int i = 0; i < num_missiles; i++)
        {
            Math::Vector pos(position + Math::Vector(float(rand()%20-10), float(rand()%20-10), 0.0f));

            Gfx::IEntityPtr entity(new MissileShotEntity(world_));
            MissileShotEntity* shot = static_cast<MissileShotEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(pos);
            shot->Rotation(Math::Vector(0.0f, rotation+float((rand()%angle)-half_angle), 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            shot_positions->push_back(pos);
        }

        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool MissilesWeapon::IncUpgrade()
{
    // return true if upgraded, false if already at max upgrade
    if(upgrade_ == 3)
    {
        return false;
    }
    ++upgrade_;
    return true;
}
