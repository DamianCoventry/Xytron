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
#include "railweapon.h"
#include "iworld.h"
#include "railshotentity.h"
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////////////////
RailWeapon::RailWeapon(IWorld* world, Gfx::IEntity* owner)
: IWeapon(PickupEntity::PICKUP_GUN_RAIL, world, owner)
, reload_time_(0)
, upgrade_(0)
, name_("Rail")
{
}

///////////////////////////////////////////////////////////////////////////////
void RailWeapon::BindResources(Util::ResourceContext* resources)
{
    snd_shoot_ = resources->FindSound("Sounds/RailShot.wav");
}

///////////////////////////////////////////////////////////////////////////////
bool RailWeapon::Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions)
{
    unsigned long now = timeGetTime();
    if(now - reload_time_ >= 500)       // 2 shots a second
    {
        reload_time_ = now;
        shot_positions->clear();

        snd_shoot_->Play2d();

        unsigned int damage = (upgrade_+1) * 4;

        float y = position.y_ - 62.5f;
        while(y > -62.5f)
        {
            Gfx::IEntityPtr entity(new RailShotEntity(world_));
            RailShotEntity* shot = static_cast<RailShotEntity*>(entity.get());
            shot->BindResources(world_->ResourceContext(), upgrade_);
            shot->Damage(damage);
            shot->Position(Math::Vector(position.x_, y, 0.0f));
            world_->PlayerShotsEntityList().push_back(entity);

            y -= 125.0f;
        }

        shot_positions->push_back(position);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool RailWeapon::IncUpgrade()
{
    // return true if upgraded, false if already at max upgrade
    if(upgrade_ == 3)
    {
        return false;
    }
    ++upgrade_;
    return true;
}
