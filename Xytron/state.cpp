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
#include "state.h"
#include "../timer/manager.h"

///////////////////////////////////////////////////////////////////////////////
State::~State()
{
    timer_manager_->Remove(this);
}

///////////////////////////////////////////////////////////////////////////////
void State::OnStateBegin()
{
}

///////////////////////////////////////////////////////////////////////////////
void State::OnStateEnd()
{
    timer_manager_->Remove(this);
}

///////////////////////////////////////////////////////////////////////////////
void State::AddTimeout(unsigned int id, unsigned long timeout)
{
    timer_manager_->Add(this, id, timeout);
}

///////////////////////////////////////////////////////////////////////////////
void State::RemoveTimeout(unsigned int id)
{
    timer_manager_->Remove(this, id);
}
