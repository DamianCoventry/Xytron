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
#include "nukeweapon.h"
#include "iworld.h"
#include "nukeshotentity.h"
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////
NukeWeapon::NukeWeapon(IWorld* world, Gfx::IEntity* owner)
: IWeapon(PickupEntity::PICKUP_BOMB_NUKE, world, owner)
, detonated_(false)
, name_("Nuke")
{
}

///////////////////////////////////////////////////////////////////////////////
void NukeWeapon::BindResources(Util::ResourceContext* resources)
{
    snd_shoot_ = resources->FindSound("Sounds/NukeShoot.wav");
}

///////////////////////////////////////////////////////////////////////////////
bool NukeWeapon::Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    if(!detonated_)
    {
        snd_shoot_->Play2d();

        Gfx::IEntityPtr entity(new NukeShotEntity(world_));
        NukeShotEntity* shot = static_cast<NukeShotEntity*>(entity.get());
        shot->BindResources(world_->ResourceContext());
        shot->Position(position);
        world_->ExplosionsEntityList().push_back(entity);      // Add it to "world_->ExplosionsEntityList()" so that it won't collide with anything

        detonated_ = true;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool NukeWeapon::IncUpgrade()
{
    // return true if upgraded, false if already at max upgrade
    return true;
}
