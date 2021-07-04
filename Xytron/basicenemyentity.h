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
#ifndef INCLUDED_BASICENEMYENTITY
#define INCLUDED_BASICENEMYENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

///////////////////////////////////////////////////////////////////////////////
struct BasicEnemyEntityEvents
{
    virtual void OnEnemyDestroyed(Gfx::IEntityPtr enemy_ship) = 0;
    virtual void OnEnemyExpired(Gfx::IEntityPtr enemy_ship) = 0;
    virtual void OnEnemyFreed(Gfx::IEntityPtr enemy_ship) = 0;
};

struct IWorld;
class PlayerEntity;
class BasicShotEntity;

///////////////////////////////////////////////////////////////////////////////
class BasicEnemyEntity
    : public Gfx::IEntity
{
public:
    BasicEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    virtual void BindResources(Util::ResourceContext* resources);

    virtual void Think(float time_delta);
    virtual void Move(float time_delta);
    virtual void Draw2d(const Gfx::Graphics& g);
    virtual void CollideWithPlayer(Gfx::IEntityPtr player);

    void StartTimeout(float timeout)                    { wait_time_ = 0.0f; wait_threshold_ = timeout; }

    void MinShootTimeout(unsigned int min_shoot);
    void MaxShootTimeout(unsigned int max_shoot);

    void SetMovementSpeed(float speed, float rotation)  { speed_ = speed; rotation_.y_ = rotation; }

    void ShotSpeed(float speed)                         { shot_speed_ = speed; }
    void MaxSpeed(float max_speed)                      { max_speed_ = max_speed; }

    unsigned long Points() const                        { return points_; }
    void Points(unsigned long points)                   { points_ = points; }

    void HitPoints(unsigned int hit_points)             { hit_points_ = hit_points; }
    virtual bool TakeDamage(unsigned int damage);

    void WayPoints(const Math::VectorList& way_points)   { way_points_ = way_points; }
    const Math::VectorList& WayPoints() const            { return way_points_; }

    virtual void Kill();
    void OnFree();

    virtual float BbWidth() const       { return 50.0f; }
    virtual float BbHeight() const      { return 50.0f; }
    virtual float BbHalfWidth() const   { return 25.0f; }
    virtual float BbHalfHeight() const  { return 25.0f; }

protected:
    bool ClassifyRotationDirection(const Math::Vector& point) const;     // Returns true if rotation right is required, false if left
    virtual void ShootThink(float time_delta);
    virtual void SelfDestructThink(float time_delta);

protected:
    BasicEnemyEntityEvents* event_handler_;

    IWorld*             world_;
    Gfx::ImageAnimPtr   imgani_ship_;
    Math::VectorList    way_points_;
    Math::Vector        rotation_;

    std::vector<Aud::SoundPtr> snd_explosions_;
    Aud::SoundPtr snd_shoot_;

    float accel_;
    float speed_;
    float max_speed_;
    float rotate_speed_;
    float shot_speed_;
    float shoot_time_;
    float shoot_threshold_;
    float self_destruct_time_;
    float wait_time_;
    float wait_threshold_;
    float waypoint_threshold_;

    unsigned int current_way_point_;
    unsigned int min_shoot_threshold_;
    unsigned int max_shoot_threshold_;
    unsigned int hit_points_;

    bool turning_right_;
    unsigned long points_;

    enum State { STATE_STARTTIMEOUT=0, STATE_DECIDING=1, STATE_TURNING=2, STATE_MOVING=3 };
    unsigned int state_;
};

#endif  // INCLUDED_BASICENEMYENTITY
