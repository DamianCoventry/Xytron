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
#ifndef INCLUDED_MINIBOSS2ENEMYENTITY
#define INCLUDED_MINIBOSS2ENEMYENTITY

#include "basicenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class MiniBoss2EnemyEntity
    : public BasicEnemyEntity
{

public:
    MiniBoss2EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    bool TakeDamage(unsigned int damage);
    void Kill();

    float BbWidth() const       { return 40.0f; }
    float BbHeight() const      { return 100.0f; }
    float BbHalfWidth() const   { return 20.0f; }
    float BbHalfHeight() const  { return 50.0f; }

private:
    void ShootThink(float time_delta);
    bool ClassifyRotationDirection2(const Math::Vector& point) const;

private:
    static float frame_shooting_offsets_[16];

private:
    bool rotate_dir_;
    bool facing_player_;
    float rotate_time_;
    float rotation_y_;
    float shoot_time_;
    int shoot_frame_;
    Aud::SoundPtr snd_explode_;
    Aud::SoundPtr snd_plasma_;

    unsigned int num_shots_;
    float plasma_reload_time_;

    enum ShootState { SS_RAIL, SS_PLASMA };
    ShootState shoot_state_;
};

#endif  // INCLUDED_MINIBOSS2ENEMYENTITY
