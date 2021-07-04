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
#ifndef INCLUDED_MISSILESWEAPON
#define INCLUDED_MISSILESWEAPON

#include "iweapon.h"
#include "../aud/aud.h"

///////////////////////////////////////////////////////////////////////////////
class MissilesWeapon
    : public IWeapon
{
public:
    MissilesWeapon(IWorld* world, Gfx::IEntity* owner);

    void BindResources(Util::ResourceContext* resources);

    bool Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions);
    bool IncUpgrade();
    unsigned int GetUpgradeLevel() const        { return upgrade_; }
    void SetUpgradeLevel(unsigned int level)    { upgrade_ = level; }

    std::string Name() const { return name_; }

private:
    std::string name_;
    unsigned long reload_time_;
    unsigned int upgrade_;
    Aud::SoundPtr snd_shoot0_;
    Aud::SoundPtr snd_shoot1_;
};

#endif  // INCLUDED_MISSILESWEAPON
