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
#ifndef INCLUDED_MINIBOSS0ENEMYENTITY
#define INCLUDED_MINIBOSS0ENEMYENTITY

#include "basicenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class MiniBoss0EnemyEntity
    : public BasicEnemyEntity
{
public:
    MiniBoss0EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    void Kill();

    float BbWidth() const       { return 66.0f; }
    float BbHeight() const      { return 74.0f; }
    float BbHalfWidth() const   { return 33.0f; }
    float BbHalfHeight() const  { return 37.0f; }

private:
    void ShootThink(float time_delta);
    bool ClassifyRotationDirection2(const Math::Vector& point) const;

private:
    bool rotate_dir_;
    bool facing_player_;
    float rotate_time_;
    float rotation_y_;
    float shoot_time_;
    int shoot_frame_;

    Gfx::ImagePtr img_ship_;

    enum GunState { GS_CLOSED, GS_OPENING, GS_OPENED, GS_CLOSING };
    struct Gun
    {
        GunState state_;
        float shoot_time_;
        float shoot_threshold_;
        float reload_time_;
        bool shooting_;
        Gfx::ImageAnimPtr imgani_;
        Math::Vector offset_;
    };
    Gun guns_[6];
};

#endif  // INCLUDED_MiniBoss0ENEMYENTITY
