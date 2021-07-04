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
#ifndef INCLUDED_PLAYERENTITY
#define INCLUDED_PLAYERENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"
#include "pickupentity.h"
#include "iweapon.h"

///////////////////////////////////////////////////////////////////////////////
struct IWorld;
struct PlayerEntityEvents
{
    virtual void OnPlayerDeath() {}
};

///////////////////////////////////////////////////////////////////////////////
class PlayerEntity
    : public Gfx::IEntity
{
public:
    static const unsigned int NUM_SHIPS;
    static const unsigned int MAX_SHIPS;
    static const unsigned int FULL_HITPOINTS;
    static const std::size_t MAX_BOMBS;

public:
    PlayerEntity(PlayerEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Unload();
    void ShootPrimary();
    void ShootSecondary();

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    void SpawnNewShip();
    void MakeTempInvulnerable();
    void MakeUnshootable();

    void OnMouseMoved(int x_delta, int y_delta);

    void Kill();
    void Reset();
    bool TakeDamage(unsigned int damage);

    void CollectPickup(PickupEntity::PickupType type);

    void AwardPoints(unsigned long points)      { score_ += points; }
    void HitPoints(unsigned int hit_points)     { hit_points_ = hit_points; }

    bool Shootable() const              { return state_ == STATE_NORMAL; }
    unsigned int RemainingShips() const { return remaining_ships_; }
    unsigned int HitPoints() const      { return hit_points_; }
    unsigned long Score() const         { return score_; }

    float BbWidth() const       { return 40.0f; }
    float BbHeight() const      { return 26.0f; }
    float BbHalfWidth() const   { return 20.0f; }
    float BbHalfHeight() const  { return 13.0f; }

    IWeaponPtr PrimaryWeapon() const    { return current_weapon_; }
    const WeaponList& BombList() const  { return bomb_list_; }

private:
    void RemoveThinkableWeapon();

private:
    PlayerEntityEvents*     event_handler_;
    IWorld*                 world_;
    Gfx::ImageAnimPtr       imgani_ship_;
    Gfx::ImagePtr           img_muzzle_flash0_;
    Gfx::ImagePtr           img_muzzle_flash1_;
    Gfx::ImagePtr           img_muzzle_flash2_;
    Gfx::ImageAnimPtr       imgani_thrusterflame_;

    std::vector<Aud::SoundPtr> snd_explosions_;
    Aud::SoundPtr   snd_100points_;
    Aud::SoundPtr   snd_1000points_;
    Aud::SoundPtr   snd_extra_ship_;
    Aud::SoundPtr   snd_collect_weapon0_;
    Aud::SoundPtr   snd_collect_weapon1_;
    Aud::SoundPtr   snd_collect_weapon2_;
    Aud::SoundPtr   snd_full_hitpoints_;

    IWeaponPtr current_weapon_;
    WeaponList bomb_list_;

    enum State { STATE_NORMAL, STATE_UNSHOOTABLE, STATE_TEMPINVULNERABLE };
    State state_;

    unsigned long score_;

    float respawn_time_;
    float blink_time_;
    float thruster_time_;

    unsigned long muzzle_flash_time_;
    Math::VectorList muzzle_flash_positions_;
    std::vector<Gfx::ImagePtr> muzzle_images_;

    bool blink_;
    float bank_rotation_;

    unsigned int hit_points_;
    unsigned int remaining_ships_;

    bool bomb_reloaded_;
    float bomb_reload_time_;
};

#endif  // INCLUDED_PLAYERENTITY
