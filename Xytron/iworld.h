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
#ifndef INCLUDED_IWORLD
#define INCLUDED_IWORLD

#include <list>
#include <boost/shared_ptr.hpp>
#include "iweapon.h"

class PlayerEntity;

///////////////////////////////////////////////////////////////////////////////
namespace Math
{
struct Vector;
}

///////////////////////////////////////////////////////////////////////////////
class State;

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
class ResourceContext;
class IEntity;
class Font;
struct Color;
typedef boost::shared_ptr<IEntity> IEntityPtr;
typedef boost::shared_ptr<Font> FontPtr;
typedef std::list<IEntityPtr> EntityList;
}

///////////////////////////////////////////////////////////////////////////////
namespace Win
{
class WindowManager;
}

class Highscores;
class Settings;

///////////////////////////////////////////////////////////////////////////////
struct IWorld
{
    virtual void Clear() = 0;
    virtual void FrameBegin() = 0;
    virtual void Think(float time_delta) = 0;
    virtual void Move(float time_delta) = 0;
    virtual void Collide() = 0;
    virtual void Draw() = 0;
    virtual void FrameEnd() = 0;
    virtual void ToggleInvulnerability() = 0;
    virtual void SkipToNextWave() = 0;
    virtual void SkipToBoss() = 0;
    virtual void SkipToWave(unsigned int wave) = 0;
    virtual bool AllEnemyEntitiesDead() const = 0;
    virtual unsigned int NumEnemyEntities() const = 0;
    virtual void InitialiseNewGameState() = 0;
    virtual bool IncrementWave() = 0;
    virtual unsigned int GetWave() const = 0;
    virtual void SetWave(unsigned int wave) = 0;
    virtual void RespawnPlayer() = 0;
    virtual void SetPlayerPosition(const Math::Vector& position) = 0;
    virtual void PlacePlayerAtStartPosition() = 0;
    virtual PlayerEntity* Player() const = 0;
    virtual Util::ResourceContext* ResourceContext() const = 0;
    virtual Gfx::FontPtr GetFont() = 0;
    virtual WeaponList& GetThinkableWeaponList() const = 0;
    virtual Gfx::EntityList& EnemyEntityList() const = 0;
    virtual Gfx::EntityList& EnemyShotsEntityList() const = 0;
    virtual Gfx::EntityList& EnemyShipsEntityList() const = 0;
    virtual Gfx::EntityList& PlayerShotsEntityList() const = 0;
    virtual Gfx::EntityList& ExplosionsEntityList() const = 0;
    virtual Gfx::EntityList& PickupEntityList() const = 0;
    virtual void StartScreenFlash(const Gfx::Color& color, unsigned long fade_ms) = 0;
    virtual State* CurrentState() const = 0;
    virtual Win::WindowManager* WindowManager() const = 0;
    virtual Highscores& GetHighscores() const = 0;
    virtual Settings& GetSettings() const = 0;
};

#endif  // INCLUDED_IWORLD
