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
#ifndef INCLUDED_STATEPAUSED
#define INCLUDED_STATEPAUSED

#include "state.h"

///////////////////////////////////////////////////////////////////////////////
struct IDialogControl;

///////////////////////////////////////////////////////////////////////////////
class StatePaused
    : public State
{
public:
    StatePaused(IStateController* state_controller, IWorld* world, Timer::Manager* timer_manager, IDialogControl* dialog_control)
        : State(state_controller, world, timer_manager), dialog_control_(dialog_control) {}
    ~StatePaused();

    void LoadResources(Util::ResourceContext& resources);

    void OnStateBegin();
    void OnStateEnd();

    void Draw2d(const Gfx::Graphics& g);

private:
    IDialogControl* dialog_control_;
};

#endif  // INCLUDED_STATEPAUSED
