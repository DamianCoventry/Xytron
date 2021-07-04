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
#ifndef INCLUDED_BOSS1ENEMYENTITY
#define INCLUDED_BOSS1ENEMYENTITY

#include "basicenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class Boss1EnemyEntity
    : public BasicEnemyEntity
{
public:
    Boss1EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    bool TakeDamage(unsigned int damage);
    void Kill();

    float BbWidth() const       { return 160.0f; }
    float BbHeight() const      { return 90.0f; }
    float BbHalfWidth() const   { return 80.0f; }
    float BbHalfHeight() const  { return 45.0f; }

private:
    void ShootThink(float time_delta);
    bool ClassifyRotationDirection2(const Math::Vector& point) const;
    void FireShot();

private:
    bool rotate_dir_;
    bool facing_player_;
    float rotate_time_;
    float rotation_y_;

    bool alternate_;
    int imgani_current_;
    unsigned int shots_remaining_;
    unsigned int missile_index_;

    Aud::SoundPtr snd_explode_;
    Aud::SoundPtr snd_missile0_;
    Aud::SoundPtr snd_missile1_;

    Gfx::ImageAnimPtr imgani_endgunleft_;
    Gfx::ImageAnimPtr imgani_endgunright_;

    enum ShootState { SS_MISSILES, SS_MISSILE_PAUSE, SS_CANNON_OPENING, SS_CANNON_SHOOTING, SS_CANNON_CLOSING, SS_CANNON_PAUSE };
    ShootState shoot_state_;
    float ss_time_;
    float ss_threshold_;

    enum EndGunState { EGS_TURNING, EGS_SHOOTING };
    EndGunState end_gun_state_;
    float left_gun_angle_;
    float right_gun_angle_;
    float left_gun_rotate_time_;
    float right_gun_rotate_time_;
    float left_gun_dest_angle_;
    float right_gun_dest_angle_;
    float end_gun_angle_time_;
    float end_gun_angle_threshold_;
    float gun_time_;
};

#endif  // INCLUDED_BOSS1ENEMYENTITY
