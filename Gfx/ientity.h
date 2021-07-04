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
#ifndef INCLUDED_GFX_IENTITY
#define INCLUDED_GFX_IENTITY

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>
#include "../math/vector.h"

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

class Graphics;
class IEntity;

typedef boost::shared_ptr<IEntity> IEntityPtr;
typedef std::list<IEntityPtr> EntityList;

///////////////////////////////////////////////////////////////////////////////
class IEntity
    : public boost::enable_shared_from_this<IEntity>
{
public:
    IEntity() : dead_(false) {}
    virtual ~IEntity() {}

    virtual float BbWidth() const = 0;
    virtual float BbHeight() const = 0;
    virtual float BbHalfWidth() const = 0;
    virtual float BbHalfHeight() const = 0;
    virtual bool IsColliding(IEntityPtr other_entity) const;
    virtual bool IsInsideRect(float x1, float y1, float x2, float y2) const;

    virtual void Think(float time_delta) {}
    virtual void Move(float time_delta) {}
    virtual void CollideWithEnemyEntities(Gfx::EntityList& entities) {}
    virtual void CollideWithPlayer(Gfx::IEntityPtr player) {}
    virtual void Draw2d(const Graphics& g) {}
    virtual void Draw3d(const Graphics& g, float mv_matrix[16]) {}

    void Position(const Math::Vector& position)   { position_ = position; }
    const Math::Vector& Position() const          { return position_; }

    virtual void Respawn()  { dead_ = false; }
    virtual void Kill()     { dead_ = true; }
    bool Dead() const       { return dead_; }

    virtual void OnCollide(IEntityPtr other_entity) { Kill(); }
    virtual void OnFree() {}

private:
    Math::Vector position_;
    bool dead_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_IENTITY
