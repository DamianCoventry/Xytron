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
#include "stdafx.h"
#include "statepregame.h"
#include "istatecontroller.h"
#include "iworld.h"
#include "idialogcontrol.h"

#include "../gfx/gfx.h"
#include "../win/windowmanager.h"

#include <boost/bind/bind.hpp>

///////////////////////////////////////////////////////////////////////////////
StatePreGame::~StatePreGame()
{
}

///////////////////////////////////////////////////////////////////////////////
void StatePreGame::OnStateBegin()
{
    World()->Clear();
    World()->WindowManager()->GetMouseCursor().Show();
    World()->WindowManager()->GetMouseCursor().Position(Math::Vector(400.0f, 300.0f, 0.0f));
    dialog_control_->ShowMainDialog();

    if(new_highscore_)
    {
        dialog_control_->ShowScoresDialog(new_highscore_, level_, score_);
    }
}

///////////////////////////////////////////////////////////////////////////////
void StatePreGame::OnStateEnd()
{
    dialog_control_->HideAllWindows();
    World()->WindowManager()->GetMouseCursor().Hide();
}

///////////////////////////////////////////////////////////////////////////////
void StatePreGame::LoadResources(Util::ResourceContext& resources)
{
    Gfx::TexturePtr tex = resources.LoadTexture("Images/PreGame.png");
    img_ = Gfx::Image::CreateFromTexture(tex, 0, 0, 800, 600);
    img_->DrawCentered(false);
}

///////////////////////////////////////////////////////////////////////////////
void StatePreGame::Draw2d(const Gfx::Graphics& g)
{
    img_->Draw2d(g, Math::Vector());
}

///////////////////////////////////////////////////////////////////////////////
void StatePreGame::Think(float time_delta)
{
    World()->Think(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StatePreGame::Move(float time_delta)
{
    World()->Move(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StatePreGame::OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    //World()->InitialiseNewGameState();
    //StateController()->ChangeToStatePreWave(false);
}

///////////////////////////////////////////////////////////////////////////////
void StatePreGame::OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    //World()->InitialiseNewGameState();
    //StateController()->ChangeToStatePreWave(false);
}
