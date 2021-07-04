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
#include "thousandpoints.h"

///////////////////////////////////////////////////////////////////////////////
ThousandPoints::ThousandPoints()
: current_frame_(0)
, frame_time_(0.0f)
, expiring_(false)
{
}

///////////////////////////////////////////////////////////////////////////////
void ThousandPoints::BindResources(Util::ResourceContext* resources)
{
    imgani_ = resources->FindImageAnim("Images/1000Points.tga");
}

///////////////////////////////////////////////////////////////////////////////
void ThousandPoints::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }
    frame_time_ += time_delta;
    if(expiring_)
    {
        if(frame_time_ >= 0.5f)
        {
            Kill();
        }
    }
    else if(frame_time_ >= 0.075f)
    {
        frame_time_ = 0.0f;
        if(++current_frame_ >= imgani_->ImageCount())
        {
            current_frame_ = imgani_->ImageCount();
            expiring_ = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void ThousandPoints::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    if(!expiring_)
    {
        Math::Vector pos = Position();
        pos.y_ -= 50.0f * time_delta;
        Position(pos);
    }
}

///////////////////////////////////////////////////////////////////////////////
void ThousandPoints::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }
    imgani_->CurrentFrame(current_frame_);
    imgani_->Draw2d(g, Position());
}
