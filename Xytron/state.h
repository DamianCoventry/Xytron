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
#ifndef INCLUDED_STATE
#define INCLUDED_STATE

#include <boost/shared_ptr.hpp>

#include "../timer/ihandler.h"
#include "../util/resourcecontext.h"

///////////////////////////////////////////////////////////////////////////////
namespace Timer
{
    class Manager;
}

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
    class Graphics;
    class ResourceContext;
    class IEntity;
    typedef boost::shared_ptr<IEntity> IEntityPtr;
}

struct IStateController;
struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class State
    : public Timer::IHandler
{
public:
    State(IStateController* state_controller, IWorld* world, Timer::Manager* timer_manager)
        : state_controller_(state_controller), world_(world), timer_manager_(timer_manager) {}
    virtual ~State();

    virtual void LoadResources(Util::ResourceContext& resources) = 0;

    virtual void OnStateBegin();
    virtual void OnStateEnd();

    virtual void FrameBegin() {}
    virtual void Think(float time_delta) {}
    virtual void Move(float time_delta) {}
    virtual void Collide() {}
    virtual void Draw2d(const Gfx::Graphics& g) {}
    virtual void Draw3d(const Gfx::Graphics& g, float mv_matrix[16]) {}
    virtual void FrameEnd() {}

    virtual void OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnMouseMoved(int x_delta, int y_delta) {}

    // Implementation of Timer::IHandler
    virtual void OnTimeout(int id) {}

    virtual void OnPlayerDeath() {}

    virtual void SkipToBoss() {}

protected:
    IStateController* StateController() const   { return state_controller_; }
    IWorld* World() const                       { return world_; }

    void AddTimeout(unsigned int id, unsigned long timeout);
    void RemoveTimeout(unsigned int id);

private:
    IStateController* state_controller_;
    IWorld* world_;
    Timer::Manager* timer_manager_;
};

typedef boost::shared_ptr<State> StatePtr;

#endif  // INCLUDED_STATE
