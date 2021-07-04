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
#include "stategamewon.h"
#include "istatecontroller.h"
#include "iworld.h"
#include "highscores.h"
#include "playerentity.h"
#include "../gfx/gfx.h"

///////////////////////////////////////////////////////////////////////////////
StateGameWon::~StateGameWon()
{
}

///////////////////////////////////////////////////////////////////////////////
void StateGameWon::LoadResources(Util::ResourceContext& resources)
{
    Gfx::TexturePtr tex = resources.LoadTexture("Images/GameWon.tga");
    img_ = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);

    sound_ = resources.FindSound("Sounds/GameWon.wav");
}

///////////////////////////////////////////////////////////////////////////////
void StateGameWon::OnStateBegin()
{
    World()->Clear();
    AddTimeout(0, 10000);
    sound_->Play2d();
}

///////////////////////////////////////////////////////////////////////////////
void StateGameWon::Think(float time_delta)
{
    World()->Think(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StateGameWon::Move(float time_delta)
{
    World()->Move(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StateGameWon::Draw2d(const Gfx::Graphics& g)
{
    World()->Draw();
    img_->Draw2d(g, Math::Vector(400.0f, 300.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
void StateGameWon::OnTimeout(int id)
{
    StateController()->ChangeToStatePreGame(
        World()->GetHighscores().IsHighscore(World()->Player()->Score()),
        World()->GetWave(),
        World()->Player()->Score());
}
