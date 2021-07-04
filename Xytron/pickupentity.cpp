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
#include "pickupentity.h"
#include "playerentity.h"
#include "basicshotexplosionentity.h"
#include "iworld.h"

///////////////////////////////////////////////////////////////////////////////
PickupEntity::PickupEntity(PickupType type, IWorld* world)
: world_(world)
, rotation_(0.0f)
, rotate_speed_(150.0f)
, way_point_timeout_(0.0f)
, pickup_change_timeout_(0.0f)
, move_speed_(150.0f)
, current_way_point_(0)
, pickup_type_(type)
, turning_right_(false)
, state_(STATE_DECIDING)
, current_frame_(0)
, frame_time_(0.0f)
{
    for(int i = 0; i < 16; i++)
    {
        switch(rand()%4)
        {
        case 0:
            way_points_.push_back(Math::Vector(400.0f + float((rand()%128)-64), 150.0f + float((rand()%128)-64), 0.0f));
            break;
        case 1:
            way_points_.push_back(Math::Vector(600.0f + float((rand()%128)-64), 300.0f + float((rand()%128)-64), 0.0f));
            break;
        case 2:
            way_points_.push_back(Math::Vector(400.0f + float((rand()%128)-64), 450.0f + float((rand()%128)-64), 0.0f));
            break;
        case 3:
            way_points_.push_back(Math::Vector(200.0f + float((rand()%128)-64), 300.0f + float((rand()%128)-64), 0.0f));
            break;
        }
    }
    way_points_.push_back(Math::Vector(100.0f+float(rand()%600), -100.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
void PickupEntity::BindResources(Util::ResourceContext* resources)
{
    std::string filename;
    switch(pickup_type_)
    {
    case PICKUP_GUN_CANNON:             filename = "Images/PickupGunCannon.tga"; break;
    case PICKUP_GUN_LASER:              filename = "Images/PickupGunLaser.tga"; break;
    case PICKUP_GUN_MISSILES:            filename = "Images/PickupGunMissiles.tga"; break;
    case PICKUP_GUN_PLASMA:             filename = "Images/PickupGunPlasma.tga"; break;
    case PICKUP_GUN_RAIL:               filename = "Images/PickupGunRail.tga"; break;
    case PICKUP_BOMB_NUKE:              filename = "Images/PickupBombNuke.tga"; break;
    case PICKUP_BOMB_SCATTER:           filename = "Images/PickupBombScatter.tga"; break;
    case PICKUP_BOMB_FUNKY:             filename = "Images/PickupBombFunky.tga"; break;
    case PICKUP_BOMB_EMP:               filename = "Images/PickupBombEmp.tga"; break;
    case PICKUP_BONUS_100POINTS:        filename = "Images/PickupBonus100points.tga"; break;
    case PICKUP_BONUS_1000POINTS:       filename = "Images/PickupBonus1000points.tga"; break;
    case PICKUP_BONUS_FULLHITPOINTS:    filename = "Images/PickupBonusFullhitpoints.tga"; break;
    case PICKUP_BONUS_EXTRASHIP:        filename = "Images/PickupBonusExtraship.tga"; break;
    }
    imgani_pickup_ = resources->FindImageAnim(filename);
}

///////////////////////////////////////////////////////////////////////////////
void PickupEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }
    switch(state_)
    {
    case STATE_DECIDING:
        turning_right_      = ClassifyRotationDirection(way_points_[current_way_point_]);
        state_              = STATE_TURNING;
        way_point_timeout_  = 0.0f;
        break;
    case STATE_TURNING:
        {
            way_point_timeout_ += time_delta;

            // Rotate towards the next waypoint
            if(turning_right_)
            {
                rotation_ += rotate_speed_*time_delta;
                if(rotation_ >= 360.0f) rotation_ -= 360.0f;
            }
            else
            {
                rotation_ -= rotate_speed_*time_delta;
                if(rotation_ < 0.0f) rotation_ += 360.0f;
            }

            // Are we facing it yet?
            if(turning_right_ != ClassifyRotationDirection(way_points_[current_way_point_]))
            {
                state_              = STATE_MOVING;
                way_point_timeout_  = 0.0f;
            }
            else if(way_point_timeout_ >= 3.0f)
            {
                way_point_timeout_ = 0.0f;

                if(++current_way_point_ >= int(way_points_.size()))
                {
                    Kill();
                }
                else
                {
                    turning_right_ = ClassifyRotationDirection(way_points_[current_way_point_]);
                }
            }

            break;
        }
    case STATE_MOVING:
        {
            way_point_timeout_ += time_delta;

            Math::Vector to_waypoint(way_points_[current_way_point_] - Position());
            float mag = to_waypoint.Magnitude();
            if(mag <= 50.0f || way_point_timeout_ >= 3.0f)
            {
                if(++current_way_point_ >= int(way_points_.size()))
                {
                    Kill();
                }
                else
                {
                    state_              = STATE_DECIDING;
                    way_point_timeout_  = 0.0f;
                }
            }
            else if(mag >= 2000.0f)
            {
                Kill();
            }
            break;
        }
    }

    frame_time_ += time_delta;
    if(frame_time_ >= 0.1f)
    {
        frame_time_ = 0.0f;

        if(++current_frame_ >= imgani_pickup_->ImageCount())
        {
            current_frame_ = 0;
        }
    }

    if(pickup_type_ >= PICKUP_GUN_CANNON && pickup_type_ <= PICKUP_GUN_RAIL)
    {
        pickup_change_timeout_ += time_delta;
        if(pickup_change_timeout_ >= 3.0f)
        {
            pickup_change_timeout_ = 0.0f;

            int i = (int)pickup_type_;
            if(++i > PICKUP_GUN_RAIL)
            {
                i = PICKUP_GUN_CANNON;
            }
            pickup_type_ = (PickupType)i;
            BindResources(world_->ResourceContext());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PickupEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }

    Math::Vector pos = Position();
    pos.x_ += (move_speed_*time_delta) * -sinf(rotation_*Math::DTOR);
    pos.y_ += (move_speed_*time_delta) * cosf(rotation_*Math::DTOR);
    Position(pos);
}

///////////////////////////////////////////////////////////////////////////////
void PickupEntity::CollideWithPlayer(PlayerEntity* player)
{
    if(Dead())
    {
        return;
    }
    if(IsColliding(player->shared_from_this()))
    {
        Kill();
        player->CollectPickup(pickup_type_);

        switch(pickup_type_)
        {
        case PickupEntity::PICKUP_GUN_CANNON:
        case PickupEntity::PICKUP_GUN_LASER:
        case PickupEntity::PICKUP_GUN_MISSILES:
        case PickupEntity::PICKUP_GUN_PLASMA:
        case PickupEntity::PICKUP_GUN_RAIL:
            {
                // A weapon was picked up so add an explosion of red pixels.
                Gfx::IEntityPtr entity(new BasicShotExplosionEntity(Position(), 30, 220, 255, 10, 30, 10, 30));
                BasicShotExplosionEntity* explosion = static_cast<BasicShotExplosionEntity*>(entity.get());
                explosion->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                break;
            }
        case PickupEntity::PICKUP_BOMB_NUKE:
        case PickupEntity::PICKUP_BOMB_SCATTER:
        case PickupEntity::PICKUP_BOMB_FUNKY:
        case PickupEntity::PICKUP_BOMB_EMP:
            {
                // A bomb was picked up so add an explosion of green pixels.
                Gfx::IEntityPtr entity(new BasicShotExplosionEntity(Position(), 30, 10, 30, 220, 255, 10, 30));
                BasicShotExplosionEntity* explosion = static_cast<BasicShotExplosionEntity*>(entity.get());
                explosion->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                break;
            }
        case PickupEntity::PICKUP_BONUS_100POINTS:
        case PickupEntity::PICKUP_BONUS_1000POINTS:
        case PickupEntity::PICKUP_BONUS_FULLHITPOINTS:
        case PickupEntity::PICKUP_BONUS_EXTRASHIP:
            {
                // An item was picked up so add an explosion of blue pixels.
                Gfx::IEntityPtr entity(new BasicShotExplosionEntity(Position(), 30, 10, 30, 150, 255, 220, 255));
                BasicShotExplosionEntity* explosion = static_cast<BasicShotExplosionEntity*>(entity.get());
                explosion->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                break;
            }
        }
        }
}

///////////////////////////////////////////////////////////////////////////////
void PickupEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }
    imgani_pickup_->CurrentFrame(current_frame_);
    imgani_pickup_->Draw2d(g, Position());
}

///////////////////////////////////////////////////////////////////////////////
bool PickupEntity::ClassifyRotationDirection(const Math::Vector& point) const
{
    Math::Vector to_waypoint(point - Position());

    Math::Vector forwards;
    forwards.x_ = -sinf(rotation_*Math::DTOR);
    forwards.y_ = cosf(rotation_*Math::DTOR);

    Math::Vector up(0.0f, 0.0f, 1.0f);
    Math::Vector right(forwards.CrossProduct(up));

    float dot = right.DotProduct(to_waypoint);
    return (dot < 0.0f);
}
