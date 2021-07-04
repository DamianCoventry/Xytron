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
#include "statepaused.h"
#include "istatecontroller.h"
#include "iworld.h"
#include "playerentity.h"
#include "idialogcontrol.h"

#include "../win/windowmanager.h"
#include "../gfx/gfx.h"

///////////////////////////////////////////////////////////////////////////////
StatePaused::~StatePaused()
{
}

///////////////////////////////////////////////////////////////////////////////
void StatePaused::LoadResources(Util::ResourceContext& resources)
{
}

///////////////////////////////////////////////////////////////////////////////
void StatePaused::OnStateBegin()
{
    World()->WindowManager()->GetMouseCursor().Show();
    World()->WindowManager()->GetMouseCursor().Position(Math::Vector(400.0f, 300.0f, 0.0f));
    dialog_control_->ShowInGameDialog();
}

///////////////////////////////////////////////////////////////////////////////
void StatePaused::OnStateEnd()
{
    dialog_control_->HideAllWindows();
    World()->WindowManager()->GetMouseCursor().Hide();

    State::OnStateEnd();
}

///////////////////////////////////////////////////////////////////////////////
void StatePaused::Draw2d(const Gfx::Graphics& g)
{
    World()->Draw();
}
