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
#include "scatterweapon.h"
#include "iworld.h"
#include "scattershotentity.h"
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const int MAX_BOMBS = 8;
}

///////////////////////////////////////////////////////////////////////////////
ScatterWeapon::ScatterWeapon(IWorld* world, Gfx::IEntity* owner)
: IWeapon(PickupEntity::PICKUP_BOMB_SCATTER, world, owner)
, detonated_(false)
, name_("Scatter")
{
}

///////////////////////////////////////////////////////////////////////////////
void ScatterWeapon::BindResources(Util::ResourceContext* resources)
{
    snd_shoot_ = resources->FindSound("Sounds/FunkyShoot.wav");
}

///////////////////////////////////////////////////////////////////////////////
bool ScatterWeapon::Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    if(!detonated_)
    {
        snd_shoot_->Play2d();

        for(int i = 0; i < MAX_BOMBS; i++)
        {
            Gfx::IEntityPtr entity(new ScatterShotEntity(world_));
            ScatterShotEntity* shot = static_cast<ScatterShotEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext());
            shot->Position(position);
            world_->ExplosionsEntityList().push_back(entity);      // Add it to "world_->ExplosionsEntityList()" so that it won't collide with anything
        }

        detonated_ = true;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool ScatterWeapon::IncUpgrade()
{
    // return true if upgraded, false if already at max upgrade
    return true;
}
