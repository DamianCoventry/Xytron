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
#include "stateprewave.h"
#include "playerentity.h"
#include "istatecontroller.h"
#include "iworld.h"
#include "../gfx/gfx.h"

///////////////////////////////////////////////////////////////////////////////
StatePreWave::~StatePreWave()
{
}

///////////////////////////////////////////////////////////////////////////////
void StatePreWave::LoadResources(Util::ResourceContext& resources)
{
    Gfx::TexturePtr tex = resources.LoadTexture("Images/WaveBeginning.tga");
    img_ = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);
}

///////////////////////////////////////////////////////////////////////////////
void StatePreWave::OnStateBegin()
{
    if(World()->Player()->Dead())
    {
        World()->RespawnPlayer();
        World()->PlacePlayerAtStartPosition();
    }
    AddTimeout(0, 3000);
}

///////////////////////////////////////////////////////////////////////////////
void StatePreWave::Think(float time_delta)
{
    World()->Think(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StatePreWave::Move(float time_delta)
{
    World()->Move(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StatePreWave::Draw2d(const Gfx::Graphics& g)
{
    World()->Draw();
    img_->Draw2d(g, Math::Vector(400.0f, 300.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
void StatePreWave::OnTimeout(int id)
{
    StateController()->ChangeToStateWave(false);
}
