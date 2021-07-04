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
#ifndef INCLUDED_ROCKETENEMYENTITY
#define INCLUDED_ROCKETENEMYENTITY

#include "basicenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class RocketEnemyEntity
    : public BasicEnemyEntity
{
public:
    RocketEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 40.0f; }
    float BbHeight() const      { return 60.0f; }
    float BbHalfWidth() const   { return 20.0f; }
    float BbHalfHeight() const  { return 30.0f; }

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
    Aud::SoundPtr snd_shoot1_;
};

#endif  // INCLUDED_ROCKETENEMYENTITY
