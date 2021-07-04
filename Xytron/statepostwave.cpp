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
#include "statepostwave.h"
#include "istatecontroller.h"
#include "iworld.h"
#include "../gfx/gfx.h"

///////////////////////////////////////////////////////////////////////////////
StatePostWave::~StatePostWave()
{
}

///////////////////////////////////////////////////////////////////////////////
void StatePostWave::LoadResources(Util::ResourceContext& resources)
{
    Gfx::TexturePtr tex = resources.LoadTexture("Images/WaveCompleted.tga");
    img_ = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);
}

///////////////////////////////////////////////////////////////////////////////
void StatePostWave::OnStateBegin()
{
    World()->Clear();
    AddTimeout(0, 3000);
}

///////////////////////////////////////////////////////////////////////////////
void StatePostWave::Think(float time_delta)
{
    World()->Think(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StatePostWave::Move(float time_delta)
{
    World()->Move(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StatePostWave::Draw2d(const Gfx::Graphics& g)
{
    World()->Draw();
    img_->Draw2d(g, Math::Vector(400.0f, 300.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
void StatePostWave::OnTimeout(int id)
{
    if(World()->IncrementWave())
    {
        StateController()->ChangeToStateGameWon();
    }
    else
    {
        StateController()->ChangeToStatePreWave(false);
    }
}
