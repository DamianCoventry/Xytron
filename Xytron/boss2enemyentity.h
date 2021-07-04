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
#ifndef INCLUDED_BOSS2ENEMYENTITY
#define INCLUDED_BOSS2ENEMYENTITY

#include "basicenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class Boss2EnemyEntity
    : public BasicEnemyEntity
{
public:
    Boss2EnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 64.0f; }
    float BbHeight() const      { return 150.0f; }
    float BbHalfWidth() const   { return 32.0f; }
    float BbHalfHeight() const  { return 75.0f; }

    bool TakeDamage(unsigned int damage);
    void Kill();

private:
    void ShootThink(float time_delta);
    void SpawnEntities();
    Math::Vector ChooseTopOffscreenPoint() const;
    Math::Vector ChooseTopOnscreenPoint() const;
    Math::VectorList GenerateTopOnScreenWaypoints(unsigned int num_waypoints) const;

private:
    float cannon_time_;
    float cannon_threshold_;
    float rail_time_;
    float rail_threshold_;
    float plasma_time_;
    float emit_time_;
    float emit_threshold_;

    enum EmitState { ES_CLOSED, ES_OPENING, ES_OPENED, ES_CLOSING };
    EmitState emit_state_;

    int door_frame_;
    unsigned int plasma_count_;

    Aud::SoundPtr snd_rail_;
    Aud::SoundPtr snd_plasma_;
    Aud::SoundPtr snd_explode_;
    Aud::SoundPtr snd_teleport_;
};

#endif  // INCLUDED_BOSS2ENEMYENTITY
