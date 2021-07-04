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
#include "playerentity.h"
#include "iworld.h"
#include "basicshotentity.h"
#include "playerexplosionentity.h"
#include "cannonweapon.h"
#include "empweapon.h"
#include "funkyweapon.h"
#include "laserweapon.h"
#include "nukeweapon.h"
#include "plasmaweapon.h"
#include "railweapon.h"
#include "missilesweapon.h"
#include "scatterweapon.h"
#include "thousandpoints.h"
#include "pickupentity.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

const unsigned int PlayerEntity::NUM_SHIPS      = 3;
const unsigned int PlayerEntity::MAX_SHIPS      = 6;
const unsigned int PlayerEntity::FULL_HITPOINTS = 15;
const std::size_t PlayerEntity::MAX_BOMBS       = 6;

///////////////////////////////////////////////////////////////////////////////
PlayerEntity::PlayerEntity(PlayerEntityEvents* event_handler, IWorld* world)
: event_handler_(event_handler)
, world_(world)
, state_(STATE_TEMPINVULNERABLE)
, respawn_time_(0.0f)
, blink_time_(0.0f)
, blink_(false)
, muzzle_flash_time_(0)
, hit_points_(FULL_HITPOINTS)
, remaining_ships_(NUM_SHIPS)
, bank_rotation_(0.0f)
, thruster_time_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::BindResources(Util::ResourceContext* resources)
{
    imgani_ship_ = resources->FindImageAnim("Images/PlayerShip.tga");
    imgani_ship_->CurrentFrame(8);

    img_muzzle_flash0_ = resources->FindImage("Images/MuzzleFlash0.tga");
    img_muzzle_flash1_ = resources->FindImage("Images/MuzzleFlash1.tga");
    img_muzzle_flash2_ = resources->FindImage("Images/MuzzleFlash2.tga");

    snd_explosions_.resize(5);
    snd_explosions_[0] = resources->FindSound("Sounds/ShipExplosion0.wav");
    snd_explosions_[1] = resources->FindSound("Sounds/ShipExplosion1.wav");
    snd_explosions_[2] = resources->FindSound("Sounds/ShipExplosion2.wav");
    snd_explosions_[3] = resources->FindSound("Sounds/ShipExplosion3.wav");
    snd_explosions_[4] = resources->FindSound("Sounds/ShipExplosion4.wav");

    snd_100points_ = resources->FindSound("Sounds/Pickup100Points.wav");
    snd_1000points_ = resources->FindSound("Sounds/Pickup1000Points.wav");
    snd_extra_ship_ = resources->FindSound("Sounds/PickupExtraShip.wav");
    snd_full_hitpoints_ = resources->FindSound("Sounds/PickupFullHitPoints.wav");

    snd_collect_weapon0_ = resources->FindSound("Sounds/CollectWeapon0.wav");
    snd_collect_weapon1_ = resources->FindSound("Sounds/CollectWeapon1.wav");
    snd_collect_weapon2_ = resources->FindSound("Sounds/CollectWeapon2.wav");

    imgani_thrusterflame_ = resources->FindImageAnim("Images/ThrusterFlame.tga");
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::Unload()
{
    imgani_ship_.reset();
    img_muzzle_flash0_.reset();
    img_muzzle_flash1_.reset();
    img_muzzle_flash2_.reset();
    imgani_thrusterflame_.reset();

    snd_100points_.reset();
    snd_1000points_.reset();
    snd_extra_ship_.reset();
    snd_collect_weapon0_.reset();
    snd_collect_weapon1_.reset();
    snd_collect_weapon2_.reset();
    snd_full_hitpoints_.reset();
    current_weapon_.reset();

    world_->GetThinkableWeaponList().clear();
    snd_explosions_.clear();
    bomb_list_.clear();
    muzzle_flash_positions_.clear();
    muzzle_images_.clear();
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::ShootPrimary()
{
    if(!Dead())
    {
        if(current_weapon_->Shoot(Position(), 180.0f, &muzzle_flash_positions_))
        {
            muzzle_images_.clear();

            Math::VectorList::iterator itor;
            for(itor = muzzle_flash_positions_.begin(); itor != muzzle_flash_positions_.end(); ++itor)
            {
                switch(rand()%3)
                {
                case 0: muzzle_images_.push_back(img_muzzle_flash0_); break;
                case 1: muzzle_images_.push_back(img_muzzle_flash1_); break;
                case 2: muzzle_images_.push_back(img_muzzle_flash2_); break;
                }
            }

            muzzle_flash_time_ = timeGetTime();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::ShootSecondary()
{
    if(!Dead() && bomb_reloaded_)
    {
        if(!bomb_list_.empty())
        {
            IWeaponPtr bomb = bomb_list_.front();
            bomb_list_.pop_front();

            bomb->Shoot(Position(), 180.0f, NULL);
            bomb_reloaded_      = false;
            bomb_reload_time_   = 0.0f;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::Think(float time_delta)
{
    switch(state_)
    {
    case STATE_NORMAL:
        break;
    case STATE_TEMPINVULNERABLE:
        respawn_time_ += time_delta;
        if(respawn_time_ >= 2.0f)
        {
            blink_time_ = 0.0f;
            blink_      = false;
            state_      = STATE_NORMAL;
            break;
        }
        // Fall through
    case STATE_UNSHOOTABLE:
        blink_time_ += time_delta;
        if(blink_time_ >= 0.2f)
        {
            blink_time_ = 0.0f;
            blink_      = !blink_;
        }
        break;
    }

    thruster_time_ += time_delta;
    if(thruster_time_ >= 0.1f)
    {
        thruster_time_ = 0.0f;
        imgani_thrusterflame_->IncCurrentFrame();
    }

    if(bank_rotation_ != 0.0f)
    {
        if(bank_rotation_ < 0)
        {
            bank_rotation_ += 100.0f * time_delta;
            if(bank_rotation_ > 0.0f)
            {
                bank_rotation_ = 0.0f;
            }
        }
        else if(bank_rotation_ > 0)
        {
            bank_rotation_ -= 100.0f * time_delta;
            if(bank_rotation_ < 0.0f)
            {
                bank_rotation_ = 0.0f;
            }
        }

        // The ship banks from -90° to +90° and has 17 frames.  The 8th frame
        // (ie. the frame with an index of 7) should be use for 0°.
        // Note that 180 / 16 = 11.25, that's where this magic number comes from.
        int current_frame = int(bank_rotation_ / 11.25f) + 8;
        imgani_ship_->CurrentFrame(current_frame);
    }

    // Allow only one bomb to be fired per second
    if(!bomb_reloaded_)
    {
        bomb_reload_time_ += time_delta;
        if(bomb_reload_time_ >= 1.0f)
        {
            bomb_reloaded_ = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }

    if(!muzzle_flash_positions_.empty())
    {
        unsigned long now = timeGetTime();
        if(now - muzzle_flash_time_ >= 50)
        {
            muzzle_flash_positions_.clear();
            muzzle_images_.clear();
        }
        else
        {
            Math::VectorList::iterator itor_pos;
            std::vector<Gfx::ImagePtr>::iterator itor_img = muzzle_images_.begin();
            for(itor_pos = muzzle_flash_positions_.begin(); itor_pos != muzzle_flash_positions_.end(); ++itor_pos, ++itor_img)
            {
                (*itor_img)->Draw2d(g, *itor_pos + Math::Vector(0.0f, -20.0f, 0.0f));
            }
        }
    }

    switch(state_)
    {
    case STATE_NORMAL:
        imgani_thrusterflame_->Draw2d(g, Position()+Math::Vector(0.0f, 32.0f, 0.0f));
        imgani_ship_->Draw2d(g, Position());
        break;
    case STATE_UNSHOOTABLE:
    case STATE_TEMPINVULNERABLE:
        if(blink_)
        {
            imgani_thrusterflame_->Draw2d(g, Position()+Math::Vector(0.0f, 32.0f, 0.0f));
            imgani_ship_->Draw2d(g, Position());
        }
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::SpawnNewShip()
{
    // Give the player a level 1 cannon
    current_weapon_.reset(new CannonWeapon(world_, this));
    current_weapon_->BindResources(world_->ResourceContext());

    // Clear any bombs the player might have.
    bomb_list_.clear();

    hit_points_         = FULL_HITPOINTS;
    bomb_reload_time_   = 0.0f;
    bomb_reloaded_      = true;

    MakeTempInvulnerable();
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::MakeTempInvulnerable()
{
    state_          = STATE_TEMPINVULNERABLE;
    respawn_time_   = 0.0f;
    blink_time_     = 0.0f;
    blink_          = false;

    current_weapon_->Reset();
    IEntity::Respawn();
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::MakeUnshootable()
{
    state_      = STATE_UNSHOOTABLE;
    blink_time_ = 0.0f;
    blink_      = false;
    current_weapon_->Reset();
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::OnMouseMoved(int x_delta, int y_delta)
{
    if(x_delta < 0)
    {
        bank_rotation_ -= 5.0f;
        if(bank_rotation_ < -90.0f)
        {
            bank_rotation_ = -90.0f;
        }
    }
    else if(x_delta > 0)
    {
        bank_rotation_ += 5.0f;
        if(bank_rotation_ > 90.0f)
        {
            bank_rotation_ = 90.0f;
        }
    }

    Math::Vector pos(Position());

    pos.x_ += float(x_delta);
    if(pos.x_ < 100.0f)
    {
        pos.x_ = 100.0f;
    }
    else if(pos.x_ > 700.0f)
    {
        pos.x_ = 700.0f;
    }

    pos.y_ += float(y_delta);
    if(pos.y_ < 0.0f)
    {
        pos.y_ = 0.0f;
    }
    else if(pos.y_ > 590.0f)    // if we let the player travel any further south, then
    {                           // their shots are clipped and never get fired up the screen.
        pos.y_ = 590.0f;
    }

    Position(pos);
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::Kill()
{
    RemoveThinkableWeapon();

    // The player's main explosion
    Gfx::IEntityPtr entity(new PlayerExplosionEntity(Position()));
    PlayerExplosionEntity* explosion = static_cast<PlayerExplosionEntity*>(entity.get());
    explosion->Position(Position());
    explosion->BindResources(world_->ResourceContext());
    world_->ExplosionsEntityList().push_back(entity);

    // Emit gun and bomb pickups if applicable.
    int count = current_weapon_->GetUpgradeLevel() + 1;
    while(count-- > 0)
    {
        PickupEntity::PickupType type;
        switch(rand()%5)
        {
        case 0: type = PickupEntity::PICKUP_GUN_CANNON; break;
        case 1: type = PickupEntity::PICKUP_GUN_LASER; break;
        case 2: type = PickupEntity::PICKUP_GUN_MISSILES; break;
        case 3: type = PickupEntity::PICKUP_GUN_PLASMA; break;
        case 4: type = PickupEntity::PICKUP_GUN_RAIL; break;
        }
        Gfx::IEntityPtr entity(new PickupEntity(type, world_));
        PickupEntity* pickup = static_cast<PickupEntity*>(entity.get());
        pickup->BindResources(world_->ResourceContext());
        pickup->Position(Position());
        world_->PickupEntityList().push_back(entity);
    }

    while(!bomb_list_.empty())
    {
        IWeaponPtr bomb = bomb_list_.front();
        bomb_list_.pop_front();

        Gfx::IEntityPtr entity(new PickupEntity(bomb->Type(), world_));
        PickupEntity* pickup = static_cast<PickupEntity*>(entity.get());
        pickup->BindResources(world_->ResourceContext());
        pickup->Position(Position());
        world_->PickupEntityList().push_back(entity);
    }

    if(remaining_ships_ > 0)
    {
        --remaining_ships_;
    }

    event_handler_->OnPlayerDeath();
    IEntity::Kill();
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::Reset()
{
    remaining_ships_    = NUM_SHIPS;
    hit_points_         = FULL_HITPOINTS;
    score_              = 0;
    bomb_reload_time_   = 0.0f;
    bomb_reloaded_      = true;

    // Give the player a level 1 cannon
    current_weapon_.reset(new CannonWeapon(world_, this));
    current_weapon_->BindResources(world_->ResourceContext());

    // Clear any bombs the player might have.
    bomb_list_.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool PlayerEntity::TakeDamage(unsigned int damage)
{
    switch(state_)
    {
    case STATE_NORMAL:
        world_->StartScreenFlash(Gfx::Color(1.0f, 0.0f, 0.0f), 1000);

        if(damage >= hit_points_)
        {
            hit_points_ = 0;
            snd_explosions_[rand()%5]->Play2d();
            Kill();
            return true;
        }
        hit_points_ -= damage;
        break;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::CollectPickup(PickupEntity::PickupType type)
{
    if(current_weapon_->Type() == type)
    {
        if(!current_weapon_->IncUpgrade())     // Was the weapon upgraded? Or already at max upgrade?
        {
            Gfx::IEntityPtr entity(new ThousandPoints);
            ThousandPoints* points = static_cast<ThousandPoints*>(entity.get());
            points->BindResources(world_->ResourceContext());
            points->Position(Position());
            world_->ExplosionsEntityList().push_back(entity);
            AwardPoints(1000);
            snd_1000points_->Play2d();
        }
        else
        {
            switch(rand()%2)
            {
            case 0: snd_collect_weapon0_->Play2d(); break;
            case 1: snd_collect_weapon1_->Play2d(); break;
            }
        }
        return;
    }

    unsigned current_upgrade = current_weapon_->GetUpgradeLevel();
    if(current_upgrade > 0)
    {
        current_upgrade--;
    }

    switch(type)
    {
    case PickupEntity::PICKUP_GUN_CANNON:
        {
            RemoveThinkableWeapon();
            current_weapon_.reset(new CannonWeapon(world_, this));
            current_weapon_->BindResources(world_->ResourceContext());
            current_weapon_->SetUpgradeLevel(current_upgrade);
            switch(rand()%2)
            {
            case 0: snd_collect_weapon0_->Play2d(); break;
            case 1: snd_collect_weapon1_->Play2d(); break;
            }
            break;
        }
    case PickupEntity::PICKUP_GUN_LASER:
        {
            RemoveThinkableWeapon();
            current_weapon_.reset(new LaserWeapon(world_, this));
            current_weapon_->BindResources(world_->ResourceContext());
            current_weapon_->SetUpgradeLevel(current_upgrade);
            world_->GetThinkableWeaponList().push_back(current_weapon_);
            switch(rand()%2)
            {
            case 0: snd_collect_weapon0_->Play2d(); break;
            case 1: snd_collect_weapon1_->Play2d(); break;
            }
            break;
        }
    case PickupEntity::PICKUP_GUN_MISSILES:
        {
            RemoveThinkableWeapon();
            current_weapon_.reset(new MissilesWeapon(world_, this));
            current_weapon_->BindResources(world_->ResourceContext());
            current_weapon_->SetUpgradeLevel(current_upgrade);
            switch(rand()%2)
            {
            case 0: snd_collect_weapon0_->Play2d(); break;
            case 1: snd_collect_weapon1_->Play2d(); break;
            }
            break;
        }
    case PickupEntity::PICKUP_GUN_PLASMA:
        {
            RemoveThinkableWeapon();
            current_weapon_.reset(new PlasmaWeapon(world_, this));
            current_weapon_->BindResources(world_->ResourceContext());
            current_weapon_->SetUpgradeLevel(current_upgrade);
            switch(rand()%2)
            {
            case 0: snd_collect_weapon0_->Play2d(); break;
            case 1: snd_collect_weapon1_->Play2d(); break;
            }
            break;
        }
    case PickupEntity::PICKUP_GUN_RAIL:
        {
            RemoveThinkableWeapon();
            current_weapon_.reset(new RailWeapon(world_, this));
            current_weapon_->BindResources(world_->ResourceContext());
            current_weapon_->SetUpgradeLevel(current_upgrade);
            switch(rand()%2)
            {
            case 0: snd_collect_weapon0_->Play2d(); break;
            case 1: snd_collect_weapon1_->Play2d(); break;
            }
            break;
        }
    case PickupEntity::PICKUP_BOMB_NUKE:
        {
            if(bomb_list_.size() < MAX_BOMBS)
            {
                IWeaponPtr bomb(new NukeWeapon(world_, this));
                bomb->BindResources(world_->ResourceContext());
                bomb_list_.push_back(bomb);
                snd_collect_weapon2_->Play2d();
            }
            else
            {
                Gfx::IEntityPtr entity(new ThousandPoints);
                ThousandPoints* points = static_cast<ThousandPoints*>(entity.get());
                points->BindResources(world_->ResourceContext());
                points->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                AwardPoints(1000);
                snd_1000points_->Play2d();
            }
            break;
        }
    case PickupEntity::PICKUP_BOMB_SCATTER:
        {
            if(bomb_list_.size() < MAX_BOMBS)
            {
                IWeaponPtr bomb(new ScatterWeapon(world_, this));
                bomb->BindResources(world_->ResourceContext());
                bomb_list_.push_back(bomb);
                snd_collect_weapon2_->Play2d();
            }
            else
            {
                Gfx::IEntityPtr entity(new ThousandPoints);
                ThousandPoints* points = static_cast<ThousandPoints*>(entity.get());
                points->BindResources(world_->ResourceContext());
                points->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                AwardPoints(1000);
                snd_1000points_->Play2d();
            }
            break;
        }
    case PickupEntity::PICKUP_BOMB_FUNKY:
        {
            if(bomb_list_.size() < MAX_BOMBS)
            {
                IWeaponPtr bomb(new FunkyWeapon(world_, this));
                bomb->BindResources(world_->ResourceContext());
                bomb_list_.push_back(bomb);
                snd_collect_weapon2_->Play2d();
            }
            else
            {
                Gfx::IEntityPtr entity(new ThousandPoints);
                ThousandPoints* points = static_cast<ThousandPoints*>(entity.get());
                points->BindResources(world_->ResourceContext());
                points->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                AwardPoints(1000);
                snd_1000points_->Play2d();
            }
            break;
        }
    case PickupEntity::PICKUP_BOMB_EMP:
        {
            if(bomb_list_.size() < MAX_BOMBS)
            {
                IWeaponPtr bomb(new EmpWeapon(world_, this));
                bomb->BindResources(world_->ResourceContext());
                bomb_list_.push_back(bomb);
                snd_collect_weapon2_->Play2d();
            }
            else
            {
                Gfx::IEntityPtr entity(new ThousandPoints);
                ThousandPoints* points = static_cast<ThousandPoints*>(entity.get());
                points->BindResources(world_->ResourceContext());
                points->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                AwardPoints(1000);
                snd_1000points_->Play2d();
            }
            break;
        }
    case PickupEntity::PICKUP_BONUS_100POINTS:
        {
            AwardPoints(100);
            snd_100points_->Play2d();
            break;
        }
    case PickupEntity::PICKUP_BONUS_1000POINTS:
        {
            AwardPoints(1000);
            snd_1000points_->Play2d();
            break;
        }
    case PickupEntity::PICKUP_BONUS_FULLHITPOINTS:
        {
            if(hit_points_ == FULL_HITPOINTS)
            {
                Gfx::IEntityPtr entity(new ThousandPoints);
                ThousandPoints* points = static_cast<ThousandPoints*>(entity.get());
                points->BindResources(world_->ResourceContext());
                points->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                AwardPoints(1000);
                snd_1000points_->Play2d();
            }
            else
            {
                snd_full_hitpoints_->Play2d();
                hit_points_ = FULL_HITPOINTS;
            }
            break;
        }
    case PickupEntity::PICKUP_BONUS_EXTRASHIP:
        {
            if(remaining_ships_ == MAX_SHIPS)
            {
                Gfx::IEntityPtr entity(new ThousandPoints);
                ThousandPoints* points = static_cast<ThousandPoints*>(entity.get());
                points->BindResources(world_->ResourceContext());
                points->Position(Position());
                world_->ExplosionsEntityList().push_back(entity);
                AwardPoints(1000);
                snd_1000points_->Play2d();
            }
            else
            {
                snd_extra_ship_->Play2d();
                ++remaining_ships_;
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlayerEntity::RemoveThinkableWeapon()
{
    WeaponList::iterator wep;
    for(wep = world_->GetThinkableWeaponList().begin(); wep != world_->GetThinkableWeaponList().end(); ++wep)
    {
        if(*wep == current_weapon_)
        {
            world_->GetThinkableWeaponList().erase(wep);
            break;
        }
    }
}
