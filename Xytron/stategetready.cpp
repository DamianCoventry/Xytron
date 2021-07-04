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
#include "stategetready.h"
#include "istatecontroller.h"
#include "iworld.h"
#include "playerentity.h"
#include "basicenemyentity.h"
#include "../gfx/gfx.h"

///////////////////////////////////////////////////////////////////////////////
StateGetReady::~StateGetReady()
{
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::LoadResources(Util::ResourceContext& resources)
{
    Gfx::TexturePtr tex = resources.LoadTexture("Images/GetReady3.tga");
    img_[0] = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);

    tex = resources.LoadTexture("Images/GetReady2.tga");
    img_[1] = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);

    tex = resources.LoadTexture("Images/GetReady1.tga");
    img_[2] = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::OnStateBegin()
{
    AddTimeout(0, 1000);
    count_ = 0;
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::OnStateEnd()
{
    if(respawn_player_)
    {
        World()->RespawnPlayer();
        World()->PlacePlayerAtStartPosition();
    }
    State::OnStateEnd();
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::Think(float time_delta)
{
    if(respawn_player_)
    {
        World()->Think(time_delta);
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::Move(float time_delta)
{
    if(respawn_player_)
    {
        World()->Move(time_delta);
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::Collide()
{
    if(respawn_player_)
    {
        World()->Collide();
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::Draw2d(const Gfx::Graphics& g)
{
    World()->Draw();
    if(count_ < 3)
    {
        img_[count_]->Draw2d(g, Math::Vector(400.0f, 300.0f, 0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::FrameEnd()
{
    World()->FrameEnd();
}

///////////////////////////////////////////////////////////////////////////////
void StateGetReady::OnTimeout(int id)
{
    if(++count_ > 2)
    {
        World()->AllEnemyEntitiesDead() ?
            StateController()->ChangeToStatePostWave() : StateController()->ChangeToStateWave(true);
    }
    else
    {
        AddTimeout(0, 1000);
    }
}
