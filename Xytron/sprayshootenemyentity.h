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
#ifndef INCLUDED_SPRAYSHOOTENEMYENTITY
#define INCLUDED_SPRAYSHOOTENEMYENTITY

#include "sitandshootenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class SprayShootEnemyEntity
    : public SitAndShootEnemyEntity
{
public:
    SprayShootEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void ShootThink(float time_delta);

    void NumShots(unsigned int num_shots)   { num_shots_ = num_shots; }
    void SpreadAngle(float spread_angle)    { spread_angle_ = spread_angle; }
    void ShotTimeout(float shot_timeout)    { shot_timeout_ = shot_timeout; }

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

private:
    void SetupSprayShot();

private:
    enum ShootState { SS_WAITING, SS_SHOOTING, SS_SHOTWAITING };
    ShootState shoot_state_;
    unsigned int num_shots_;
    unsigned int current_shot_;
    int rotate_frame_;
    float spread_angle_;
    float angle_delta_;
    float current_shot_angle_;
    float shot_time_;
    float shot_timeout_;
    float rotate_time_;
    bool spray_left_;

    Gfx::ImageAnimPtr imgani_shipframes_[8];
};

#endif  // INCLUDED_SPRAYSHOOTENEMYENTITY
