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
#include "railenemyentity.h"
#include "iworld.h"
#include "playerentity.h"
#include "railshotentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float ROTATE_THRESHOLD = 0.25f;
}

///////////////////////////////////////////////////////////////////////////////
RailEnemyEntity::RailEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world)
: BasicEnemyEntity(event_handler, world)
, rotate_time_(0.0f)
, rotate_dir_(false)
, facing_player_(false)
, rotation_y_(0.0f)
, rotate_frame_(0)
, shoot_frame_(0)
, frame_rotate_time_(0.0f)
, shoot_time_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void RailEnemyEntity::BindResources(Util::ResourceContext* resources)
{
    snd_explosions_.resize(5);
    snd_explosions_[0] = resources->FindSound("Sounds/ShipExplosion0.wav");
    snd_explosions_[1] = resources->FindSound("Sounds/ShipExplosion1.wav");
    snd_explosions_[2] = resources->FindSound("Sounds/ShipExplosion2.wav");
    snd_explosions_[3] = resources->FindSound("Sounds/ShipExplosion3.wav");
    snd_explosions_[4] = resources->FindSound("Sounds/ShipExplosion4.wav");

    snd_shoot_ = resources->FindSound("Sounds/RailShot.wav");

    imgani_shipframes_[0] = resources->FindImageAnim("Images/EnemyType3_0.tga");
    imgani_shipframes_[1] = resources->FindImageAnim("Images/EnemyType3_1.tga");
    imgani_shipframes_[2] = resources->FindImageAnim("Images/EnemyType3_2.tga");
    imgani_shipframes_[3] = resources->FindImageAnim("Images/EnemyType3_3.tga");
    imgani_shipframes_[4] = resources->FindImageAnim("Images/EnemyType3_4.tga");
    imgani_shipframes_[5] = resources->FindImageAnim("Images/EnemyType3_5.tga");
    imgani_shipframes_[6] = resources->FindImageAnim("Images/EnemyType3_6.tga");
    imgani_shipframes_[7] = resources->FindImageAnim("Images/EnemyType3_7.tga");
    imgani_shipframes_[8] = resources->FindImageAnim("Images/EnemyType3_8.tga");
}

///////////////////////////////////////////////////////////////////////////////
void RailEnemyEntity::Think(float time_delta)
{
    BasicEnemyEntity::Think(time_delta);

    if(facing_player_)
    {
        rotate_time_ += time_delta;
        if(rotate_time_ >= ROTATE_THRESHOLD)
        {
            rotate_time_    = 0.0f;
            facing_player_  = false;
            rotate_dir_     = ClassifyRotationDirection2(world_->Player()->Position());
        }
    }
    else
    {
        if(rotate_dir_)
        {
            rotation_y_ += rotate_speed_*time_delta;
            if(rotation_y_ >= 360.0f) rotation_y_ -= 360.0f;
        }
        else
        {
            rotation_y_ -= rotate_speed_*time_delta;
            if(rotation_y_ < 0.0f) rotation_y_ += 360.0f;
        }

        if(rotate_dir_ != ClassifyRotationDirection2(world_->Player()->Position()))
        {
            facing_player_ = true;
        }
    }

    frame_rotate_time_ += time_delta;
    if(frame_rotate_time_ >= 0.1f)
    {
        frame_rotate_time_ = 0.0f;
        if(++rotate_frame_ >= 9)
        {
            rotate_frame_ = 0;
        }
    }

    if(shoot_frame_ > 0)
    {
        shoot_time_ += time_delta;
        if(shoot_time_ >= 0.1f)
        {
            shoot_time_ = 0.0f;
            if(++shoot_frame_ >= 16)
            {
                shoot_frame_ = 0;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void RailEnemyEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead() || state_ == STATE_STARTTIMEOUT)
    {
        return;
    }
    imgani_shipframes_[rotate_frame_]->CurrentFrame(shoot_frame_);
    imgani_shipframes_[rotate_frame_]->Draw2d(g, Position());
}

///////////////////////////////////////////////////////////////////////////////
bool RailEnemyEntity::ClassifyRotationDirection2(const Math::Vector& point) const
{
    Math::Vector to_waypoint(point - Position());

    Math::Vector forwards;
    forwards.x_ = -sinf(rotation_y_*Math::DTOR);
    forwards.y_ = cosf(rotation_y_*Math::DTOR);

    Math::Vector up(0.0f, 0.0f, 1.0f);
    Math::Vector right(forwards.CrossProduct(up));

    float dot = right.DotProduct(to_waypoint);
    return (dot < 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
void RailEnemyEntity::ShootThink(float time_delta)
{
    shoot_time_ += time_delta;
    if(shoot_time_ >= shoot_threshold_)
    {
        shoot_time_ = 0.0f;
        shoot_threshold_ = float((rand()%max_shoot_threshold_)+min_shoot_threshold_)/1000.0f;

        if(!world_->Player()->Dead() && world_->Player()->Shootable())
        {
            snd_shoot_->Play2d();

            float y = Position().y_ + 62.5f;
            while(y < 662.5f)
            {
                Gfx::IEntityPtr entity(new RailShotEntity(world_));
                RailShotEntity* shot = static_cast<RailShotEntity*>(entity.get());
                shot->BindResources(world_->ResourceContext(), 1);
                shot->Damage(3);
                shot->Position(Math::Vector(Position().x_, y, 0.0f));
                world_->EnemyShotsEntityList().push_back(entity);

                y += 125.0f;
            }

            shoot_frame_ = 1;
        }
    }
}
