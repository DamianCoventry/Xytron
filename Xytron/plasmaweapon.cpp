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
#include "plasmaweapon.h"
#include "iworld.h"
#include "plasmashotentity.h"
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////
PlasmaWeapon::PlasmaWeapon(IWorld* world, Gfx::IEntity* owner)
: IWeapon(PickupEntity::PICKUP_GUN_PLASMA, world, owner)
, reload_time_(0)
, upgrade_(0)
, name_("Plasma")
{
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaWeapon::BindResources(Util::ResourceContext* resources)
{
    snd_shoot_ = resources->FindSound("Sounds/PlasmaShot.wav");
}

///////////////////////////////////////////////////////////////////////////////
bool PlasmaWeapon::Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    unsigned long now = timeGetTime();
    if(now - reload_time_ >= 100)       // 10 shots a second
    {
        reload_time_ = now;
        shot_positions->clear();

        snd_shoot_->Play2d();

        int angle;
        switch(upgrade_)
        {
        case 0: angle = 20; break;
        case 1: angle = 23; break;
        case 2: angle = 27; break;
        case 3: angle = 30; break;
        }
        int half_angle = angle/2;

        Math::Vector pos(position + Math::Vector(float(rand()%30-15), float(rand()%30-15), 0.0f));

        Gfx::IEntityPtr entity(new PlasmaShotEntity(world_));
        PlasmaShotEntity* shot = static_cast<PlasmaShotEntity*>(entity.get());
        shot->BindResources(world_->ResourceContext(), upgrade_);
        shot->Position(pos);
        shot->Rotation(Math::Vector(0.0f, rotation+float((rand()%angle)-half_angle), 0.0f));
        world_->PlayerShotsEntityList().push_back(entity);

        shot_positions->push_back(pos);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool PlasmaWeapon::IncUpgrade()
{
    // return true if upgraded, false if already at max upgrade
    if(upgrade_ == 3)
    {
        return false;
    }
    ++upgrade_;
    return true;
}
