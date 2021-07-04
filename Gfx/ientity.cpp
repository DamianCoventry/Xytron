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
#include "ientity.h"

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
bool IEntity::IsColliding(IEntityPtr other_entity) const
{
    if(position_.x_ + BbHalfWidth() < other_entity->Position().x_ - other_entity->BbHalfWidth()) return false;
    if(position_.x_ - BbHalfWidth() > other_entity->Position().x_ + other_entity->BbHalfWidth()) return false;
    if(position_.y_ + BbHalfHeight() < other_entity->Position().y_ - other_entity->BbHalfHeight()) return false;
    if(position_.y_ - BbHalfHeight() > other_entity->Position().y_ + other_entity->BbHalfHeight()) return false;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool IEntity::IsInsideRect(float x1, float y1, float x2, float y2) const
{
    if(position_.x_ + BbHalfWidth() < x1) return false;
    if(position_.x_ - BbHalfWidth() > x2) return false;
    if(position_.y_ + BbHalfHeight() < y1) return false;
    if(position_.y_ - BbHalfHeight() > y2) return false;
    return true;
}
