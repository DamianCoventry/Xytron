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
#ifndef INCLUDED_STATEPREWAVE
#define INCLUDED_STATEPREWAVE

#include "state.h"

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
    class Image;
    typedef boost::shared_ptr<Image> ImagePtr;
}

///////////////////////////////////////////////////////////////////////////////
class StatePreWave
    : public State
{
public:
    StatePreWave(IStateController* state_controller, IWorld* world, Timer::Manager* timer_manager)
        : State(state_controller, world, timer_manager), warping_(false) {}
    ~StatePreWave();

    void LoadResources(Util::ResourceContext& resources);

    void OnStateBegin();

    void Think(float time_delta);
    void Move(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    void OnTimeout(int id);
    void Warping(bool warping) { warping_ = warping; }

private:
    Gfx::ImagePtr img_;
    bool warping_;
};

#endif  // INCLUDED_STATEPREWAVE
