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
#ifndef INCLUDED_STATEGETREADY
#define INCLUDED_STATEGETREADY

#include "state.h"

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
    class Image;
    typedef boost::shared_ptr<Image> ImagePtr;
}

///////////////////////////////////////////////////////////////////////////////
class StateGetReady
    : public State
{
public:
    StateGetReady(IStateController* state_controller, IWorld* world, Timer::Manager* timer_manager)
        : State(state_controller, world, timer_manager) {}
    ~StateGetReady();

    void RespawnPlayer(bool respawn_player) { respawn_player_ = respawn_player; }

    void LoadResources(Util::ResourceContext& resources);

    void OnStateBegin();
    void OnStateEnd();

    void Think(float time_delta);
    void Move(float time_delta);
    void Collide();
    void Draw2d(const Gfx::Graphics& g);
    void FrameEnd();

    void OnTimeout(int id);

private:
    Gfx::ImagePtr img_[3];
    bool respawn_player_;
    int count_;
};

#endif  // INCLUDED_STATEGETREADY
