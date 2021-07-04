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
#ifndef INCLUDED_RAILENEMYENTITY
#define INCLUDED_RAILENEMYENTITY

#include "basicenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class RailEnemyEntity
    : public BasicEnemyEntity
{
public:
    RailEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 40.0f; }
    float BbHeight() const      { return 70.0f; }
    float BbHalfWidth() const   { return 20.0f; }
    float BbHalfHeight() const  { return 35.0f; }

private:
    void ShootThink(float time_delta);
    bool ClassifyRotationDirection2(const Math::Vector& point) const;

private:
    bool rotate_dir_;
    bool facing_player_;
    float rotate_time_;
    float rotation_y_;
    float frame_rotate_time_;
    float shoot_time_;
    int shoot_frame_;
    int rotate_frame_;

    Gfx::ImageAnimPtr imgani_shipframes_[9];
};

#endif  // INCLUDED_RAILENEMYENTITY
