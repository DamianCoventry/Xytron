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
#ifndef INCLUDED_PLASMAENEMYENTITY
#define INCLUDED_PLASMAENEMYENTITY

#include "basicenemyentity.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class PlasmaEnemyEntity
    : public BasicEnemyEntity
{
public:
    PlasmaEnemyEntity(BasicEnemyEntityEvents* event_handler, IWorld* world);

    virtual void BindResources(Util::ResourceContext* resources);

    virtual void Think(float time_delta);
    void Move(float time_delta);

    void SitMinTimeout(unsigned int min_timeout)    { min_sit_timeout_ = min_timeout; }
    void SitMaxTimeout(unsigned int max_timeout)    { max_sit_timeout_ = max_timeout; }

private:
    void ShootThink(float time_delta);
    void FireShot(float time_delta);

private:
    enum SubState { STATE_SITTING=4 };
    enum ShootState { SS_WAITING, SS_SHOOTING };

    ShootState shoot_state_;

    unsigned int min_sit_timeout_;
    unsigned int max_sit_timeout_;

    float sit_time_;
    float sit_threshold_;
    float rotate_time_;
    float plasma_reload_;
    bool rotate_dir_;
    bool facing_player_;
};

#endif  // INCLUDED_PlasmaENEMYENTITY
