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
#include "empweapon.h"
#include "iworld.h"
#include "empshotexplosionentity.h"
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////
EmpWeapon::EmpWeapon(IWorld* world, Gfx::IEntity* owner)
: IWeapon(PickupEntity::PICKUP_BOMB_EMP, world, owner)
, detonated_(false)
, name_("EMP")
{
}

///////////////////////////////////////////////////////////////////////////////
void EmpWeapon::BindResources(Util::ResourceContext* resources)
{
    snd_shoot_ = resources->FindSound("Sounds/EmpShoot.wav");
}

///////////////////////////////////////////////////////////////////////////////
bool EmpWeapon::Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    if(!detonated_)
    {
        snd_shoot_->Play2d();

        Gfx::IEntityPtr entity(new EmpShotExplosionEntity(position, world_));
        EmpShotExplosionEntity* explosion = static_cast<EmpShotExplosionEntity*>(entity.get());
        explosion->BindResources(world_->ResourceContext());
        world_->ExplosionsEntityList().push_back(entity);

        detonated_ = true;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool EmpWeapon::IncUpgrade()
{
    // return true if upgraded, false if already at max upgrade
    return true;
}
