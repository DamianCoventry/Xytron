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
#include "lasershotlargeentity.h"
#include "iworld.h"
#include "basicenemyentity.h"
#include "basicshotexplosionentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const unsigned int DAMAGE_CAUSED = 6;
}

///////////////////////////////////////////////////////////////////////////////
LaserShotLargeEntity::LaserShotLargeEntity(IWorld* world)
: world_(world)
, speed_(850.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void LaserShotLargeEntity::BindResources(Util::ResourceContext* resources)
{
    img_            = resources->FindImage("Images/LaserShotLarge.tga");
    snd_collide_    = resources->FindSound("Sounds/LaserHit.wav");
}

///////////////////////////////////////////////////////////////////////////////
void LaserShotLargeEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
void LaserShotLargeEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    Math::Vector pos = Position();
    pos.x_ += (speed_*time_delta) * -sinf(rotation_.y_*Math::DTOR);
    pos.y_ += (speed_*time_delta) * cosf(rotation_.y_*Math::DTOR);
    Position(pos);

    if(Position().x_ < 100.0f || Position().x_ >= 700.0f ||
       Position().y_ < 0.0f || Position().y_ >= 600.0f)
    {
        Kill();
    }
}

///////////////////////////////////////////////////////////////////////////////
void LaserShotLargeEntity::CollideWithEnemyEntities(Gfx::EntityList& entities)
{
    if(Dead())
    {
        return;
    }

    Gfx::EntityList::iterator itor;
    for(itor = entities.begin(); itor != entities.end(); ++itor)
    {
        if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f) && IsColliding(*itor))
        {
            OnCollide(*itor);
            BasicEnemyEntity* enemy = static_cast<BasicEnemyEntity*>(itor->get());
            if(!enemy->TakeDamage(DAMAGE_CAUSED))
            {
                // The enemy didn't die from this damage.  We'll draw some sparks to
                // indicate this shot exploded.  We don't bother drawing this sparked
                // based explosion if the enemy did explode - there'll be enough crap
                // drawn by that enemy's explosion.
                Gfx::IEntityPtr entity(new BasicShotExplosionEntity(Position(),
                                                                    20,
                                                                    75, 160,
                                                                    150, 225,
                                                                    225, 255));
                BasicShotExplosionEntity* explosion = static_cast<BasicShotExplosionEntity*>(entity.get());
                explosion->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);

                snd_collide_->Play2d();
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void LaserShotLargeEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }
    img_->Draw2d(g, Position());
}
