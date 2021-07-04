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
#ifndef INCLUDED_GFX_IWEAPON
#define INCLUDED_GFX_IWEAPON

#include <boost/shared_ptr.hpp>
#include <string>
#include <list>
#include <vector>
#include "pickupentity.h"

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
struct IWorld;
struct Vector;
typedef std::vector<Vector> VectorList;

class ResourceContext;

class IEntity;
typedef boost::shared_ptr<IEntity> IEntityPtr;
typedef std::list<IEntityPtr> EntityList;
}

///////////////////////////////////////////////////////////////////////////////
namespace Util
{
class ResourceContext;
}


///////////////////////////////////////////////////////////////////////////////
class IWeapon
{
public:
    IWeapon(PickupEntity::PickupType pickup_type, IWorld* world, Gfx::IEntity* owner)
        : pickup_type_(pickup_type), world_(world), owner_(owner) {}
    virtual ~IWeapon() {}

    virtual void BindResources(Util::ResourceContext* resources) {}

    virtual bool Shoot(const Math::Vector& position, float rotation, Math::VectorList* shot_positions) = 0;
    virtual bool IncUpgrade() = 0;             // returns true if upgraded, false if already at max upgrade

    virtual unsigned int GetUpgradeLevel() const = 0;
    virtual void SetUpgradeLevel(unsigned int level) = 0;

    virtual std::string Name() const { return std::string(); }

    virtual void Reset() {}
    virtual void Think(float time_delta) {}

    PickupEntity::PickupType Type() const { return pickup_type_; }

protected:
    IWorld*         world_;
    Gfx::IEntity*   owner_;

    PickupEntity::PickupType pickup_type_;
};

typedef boost::shared_ptr<IWeapon> IWeaponPtr;
typedef std::list<IWeaponPtr> WeaponList;

#endif  // INCLUDED_GFX_IWEAPON
