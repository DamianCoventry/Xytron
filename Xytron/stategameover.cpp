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
#include "stategameover.h"
#include "istatecontroller.h"
#include "iworld.h"
#include "highscores.h"
#include "playerentity.h"
#include "../gfx/gfx.h"

///////////////////////////////////////////////////////////////////////////////
StateGameOver::~StateGameOver()
{
}

///////////////////////////////////////////////////////////////////////////////
void StateGameOver::LoadResources(Util::ResourceContext& resources)
{
    Gfx::TexturePtr tex = resources.LoadTexture("Images/GameOver.tga");
    img_ = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);

    sound_ = resources.FindSound("Sounds/GameOver.wav");
}

///////////////////////////////////////////////////////////////////////////////
void StateGameOver::OnStateBegin()
{
    World()->Clear();
    AddTimeout(0, 5000);
    sound_->Play2d();
}

///////////////////////////////////////////////////////////////////////////////
void StateGameOver::Think(float time_delta)
{
    World()->Think(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StateGameOver::Move(float time_delta)
{
    World()->Move(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StateGameOver::Draw2d(const Gfx::Graphics& g)
{
    World()->Draw();
    img_->Draw2d(g, Math::Vector(400.0f, 300.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
void StateGameOver::OnTimeout(int id)
{
    StateController()->ChangeToStatePreGame(
        World()->GetHighscores().IsHighscore(World()->Player()->Score()),
        World()->GetWave(),
        World()->Player()->Score());
}
