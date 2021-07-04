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
#ifndef INCLUDED_PICKUPENTITY
#define INCLUDED_PICKUPENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

class PlayerEntity;
struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class PickupEntity
    : public Gfx::IEntity
{
public:
    enum PickupType { PICKUP_GUN_CANNON, PICKUP_GUN_LASER, PICKUP_GUN_MISSILES, PICKUP_GUN_PLASMA, PICKUP_GUN_RAIL,
                      PICKUP_BOMB_NUKE, PICKUP_BOMB_SCATTER, PICKUP_BOMB_FUNKY, PICKUP_BOMB_EMP,
                      PICKUP_BONUS_100POINTS, PICKUP_BONUS_1000POINTS, PICKUP_BONUS_FULLHITPOINTS, PICKUP_BONUS_EXTRASHIP };

public:
    PickupEntity(PickupType type, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Move(float time_delta);
    void CollideWithPlayer(PlayerEntity* player);
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 26.0f; }
    float BbHeight() const      { return 26.0f; }
    float BbHalfWidth() const   { return 13.0f; }
    float BbHalfHeight() const  { return 13.0f; }

private:
    bool ClassifyRotationDirection(const Math::Vector& point) const;     // Returns true if rotation right is required, false if left

private:
    IWorld*             world_;
    Gfx::ImageAnimPtr   imgani_pickup_;
    Math::VectorList    way_points_;

    bool turning_right_;
    float rotation_;
    float rotate_speed_;
    float move_speed_;
    float way_point_timeout_;
    float pickup_change_timeout_;
    unsigned int current_way_point_;

    float frame_time_;
    int current_frame_;

    PickupType pickup_type_;

    enum State { STATE_DECIDING, STATE_TURNING, STATE_MOVING };
    State state_;
};

#endif  // INCLUDED_PICKUPENTITY
