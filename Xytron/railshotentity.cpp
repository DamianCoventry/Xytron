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
#include "railshotentity.h"
#include "iworld.h"
#include "basicenemyentity.h"
#include "playerentity.h"
#include "railshotexplosionentity.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
RailShotEntity::RailShotEntity(IWorld* world)
: world_(world)
, alpha_time_(0.0f)
, alpha_(1.0f)
, damage_(3)
, upgrade_(0)
, damage_depleted_(false)
{
}

///////////////////////////////////////////////////////////////////////////////
void RailShotEntity::BindResources(Util::ResourceContext* resources, unsigned int upgrade)
{
    upgrade_ = upgrade;

    std::ostringstream oss;
    oss << "Images/RailShot" << upgrade_ << ".tga";
    img_ = resources->FindImage(oss.str());
}

///////////////////////////////////////////////////////////////////////////////
float RailShotEntity::BbWidth() const
{
    switch(upgrade_)
    {
    case 1: return 14.0f;
    case 2: return 16.0f;
    case 3: return 20.0f;
    }
    // default to upgrade 0
    return 12.0f;
}

///////////////////////////////////////////////////////////////////////////////
float RailShotEntity::BbHalfWidth() const
{
    switch(upgrade_)
    {
    case 1: return 7.0f;
    case 2: return 8.0f;
    case 3: return 10.0f;
    }
    // default to upgrade 0
    return 6.0f;
}

///////////////////////////////////////////////////////////////////////////////
void RailShotEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }

    alpha_time_ += time_delta;
    if(alpha_time_ >= 0.1f)
    {
        alpha_ -= 0.05f;
        if(alpha_ <= 0.0f)
        {
            Kill();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void RailShotEntity::CollideWithPlayer(Gfx::IEntityPtr player_entity)
{
    if(Dead())
    {
        return;
    }

    if(!damage_depleted_)
    {
        if(IsColliding(player_entity))
        {
            PlayerEntity* player = static_cast<PlayerEntity*>(player_entity.get());
            if(!player->TakeDamage(damage_))
            {
                Gfx::IEntityPtr entity(new RailShotExplosionEntity(player_entity->Position(),
                                                                   15,
                                                                   150, 255,
                                                                   150, 255,
                                                                   220, 255));
                RailShotExplosionEntity* explosion = static_cast<RailShotExplosionEntity*>(entity.get());
                explosion->BindResources(world_->ResourceContext());
                explosion->Position(player_entity->Position());
                world_->ExplosionsEntityList().push_back(entity);
            }
        }
        damage_depleted_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void RailShotEntity::CollideWithEnemyEntities(Gfx::EntityList& entities)
{
    if(Dead())
    {
        return;
    }

    if(!damage_depleted_)
    {
        Gfx::EntityList::iterator itor;
        for(itor = entities.begin(); itor != entities.end(); ++itor)
        {
            if(IsInsideRect(100.0f, 0.0f, 700.0f, 600.0f) && IsColliding(*itor))
            {
                BasicEnemyEntity* enemy = static_cast<BasicEnemyEntity*>(itor->get());
                enemy->TakeDamage(damage_);

                Gfx::IEntityPtr entity(new RailShotExplosionEntity(enemy->Position(),
                                                                   15,
                                                                   150, 255,
                                                                   150, 255,
                                                                   220, 255));
                RailShotExplosionEntity* explosion = static_cast<RailShotExplosionEntity*>(entity.get());
                explosion->BindResources(world_->ResourceContext());
                explosion->Position(enemy->Position());
                world_->ExplosionsEntityList().push_back(entity);
            }
        }
        damage_depleted_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void RailShotEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }
    img_->Draw2d(g, Position(), alpha_);
}
