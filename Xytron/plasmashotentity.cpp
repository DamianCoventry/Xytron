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
#include "plasmashotentity.h"
#include "iworld.h"
#include "basicenemyentity.h"
#include "plasmashotexplosionentity.h"
#include "playerentity.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
PlasmaShotEntity::PlasmaShotEntity(IWorld* world)
: world_(world)
, speed_(500.0f)
, damage_(2)
, upgrade_(0)
{
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaShotEntity::BindResources(Util::ResourceContext* resources, unsigned int upgrade)
{
    std::ostringstream oss;
    oss << "Images/PlasmaShot" << upgrade << ".tga";
    img_ = resources->FindImage(oss.str());

    upgrade_ = upgrade;
    damage_ = (2*upgrade_) + 2;

    snd_collide_ = resources->FindSound("Sounds/PlasmaHit.wav");
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaShotEntity::Move(float time_delta)
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
void PlasmaShotEntity::CollideWithPlayer(Gfx::IEntityPtr player_entity)
{
    if(Dead())
    {
        return;
    }

    if(IsColliding(player_entity))
    {
        OnCollide(player_entity);

        PlayerEntity* player = static_cast<PlayerEntity*>(player_entity.get());
        if(!player->TakeDamage(damage_))
        {
            Gfx::IEntityPtr entity(new PlasmaShotExplosionEntity(Position(), upgrade_ < 2));
            PlasmaShotExplosionEntity* explosion = static_cast<PlasmaShotExplosionEntity*>(entity.get());
            explosion->BindResources(world_->ResourceContext());
            explosion->Position(Position());
            world_->ExplosionsEntityList().push_back(entity);

            snd_collide_->Play2d();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaShotEntity::CollideWithEnemyEntities(Gfx::EntityList& entities)
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
            if(!enemy->TakeDamage(damage_))
            {
                // The enemy didn't die from this damage.  We'll draw some sparks to
                // indicate this shot exploded.  We don't bother drawing this sparked
                // based explosion if the enemy did explode - there'll be enough crap
                // drawn by that enemy's explosion.
                Gfx::IEntityPtr entity(new PlasmaShotExplosionEntity(Position(), upgrade_ < 2));
                PlasmaShotExplosionEntity* explosion = static_cast<PlasmaShotExplosionEntity*>(entity.get());
                explosion->BindResources(world_->ResourceContext());
                explosion->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);

                snd_collide_->Play2d();
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaShotEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }
    img_->Draw2d(g, Position());
}

///////////////////////////////////////////////////////////////////////////////
float PlasmaShotEntity::BbWidth() const
{ 
    switch(upgrade_)
    {
    case 1: return 20.0f;
    case 2: return 25.0f;
    case 3: return 25.0f;
    }
    return 35.0f;
}

///////////////////////////////////////////////////////////////////////////////
float PlasmaShotEntity::BbHeight() const
{ 
    switch(upgrade_)
    {
    case 1: return 20.0f;
    case 2: return 25.0f;
    case 3: return 25.0f;
    }
    return 35.0f;
}

///////////////////////////////////////////////////////////////////////////////
float PlasmaShotEntity::BbHalfWidth() const
{ 
    switch(upgrade_)
    {
    case 1: return 10.0f;
    case 2: return 12.5f;
    case 3: return 12.5f;
    }
    return 17.5f;
}

///////////////////////////////////////////////////////////////////////////////
float PlasmaShotEntity::BbHalfHeight() const
{ 
    switch(upgrade_)
    {
    case 1: return 10.0f;
    case 2: return 12.5f;
    case 3: return 12.5f;
    }
    return 17.5f;
}
