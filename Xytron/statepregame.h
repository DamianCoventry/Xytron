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
#ifndef INCLUDED_STATEPREGAME
#define INCLUDED_STATEPREGAME

#include "state.h"
#include "../win/buttonwindow.h"

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{
    class Image;
    typedef boost::shared_ptr<Image> ImagePtr;
}

struct IDialogControl;

///////////////////////////////////////////////////////////////////////////////
class StatePreGame
    : public State
{
public:
    StatePreGame(IStateController* state_controller, IWorld* world, Timer::Manager* timer_manager, IDialogControl* dialog_control)
        : State(state_controller, world, timer_manager), dialog_control_(dialog_control)
        , new_highscore_(false), level_(0), score_(0) {}
    ~StatePreGame();

    void OnStateBegin();
    void OnStateEnd();

    void EnterNewHighscore(bool new_highscore = false, unsigned int level = 0, unsigned long score = 0)
    { new_highscore_ = new_highscore; level_ = level; score_ = score; }

    void LoadResources(Util::ResourceContext& resources);

    void Think(float time_delta);
    void Move(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    void OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);

private:
    Gfx::ImagePtr img_;
    IDialogControl* dialog_control_;
    bool new_highscore_;
    unsigned int level_;
    unsigned long score_;
};

#endif  // INCLUDED_STATEPREGAME
