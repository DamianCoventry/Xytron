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
#ifndef INCLUDED_BOSS0ENEMYENTITY
#define INCLUDED_BOSS0ENEMYENTITY

#include "basicenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class Boss0EnemyEntity
    : public BasicEnemyEntity
{
public:
    Boss0EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 66.0f; }
    float BbHeight() const      { return 74.0f; }
    float BbHalfWidth() const   { return 33.0f; }
    float BbHalfHeight() const  { return 37.0f; }

    bool TakeDamage(unsigned int damage);
    void Kill();

private:
    void ShootThink(float time_delta);
    bool ClassifyRotationDirection2(const Math::Vector& point) const;

private:
    bool gun_rotate_dir_;
    bool gun_facing_player_;
    bool plasma_shooting_;
    bool reloading_;

    float gun_rotate_time_;
    float shoot_rotation_;
    float shoot_time_;
    float plasma_time_;
    float gun_anim_time_;
    float plasma_threshold_;
    float reload_time_;

    int shoot_frame_;
    int gun_anim_index_;

    Aud::SoundPtr snd_plasma_;
    Aud::SoundPtr snd_explode_;

    Gfx::ImagePtr img_shipbot_;
    Gfx::ImageAnimPtr img_shiptop_[36];
};

#endif  // INCLUDED_BOSS0ENEMYENTITY
