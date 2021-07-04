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
#ifndef INCLUDED_THOUSANDPOINTS
#define INCLUDED_THOUSANDPOINTS

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

///////////////////////////////////////////////////////////////////////////////
class ThousandPoints
    : public Gfx::IEntity
{
public:
    ThousandPoints();

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Move(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 64.0f; }
    float BbHeight() const      { return 24.0f; }
    float BbHalfWidth() const   { return 32.0f; }
    float BbHalfHeight() const  { return 12.0f; }

private:
    int current_frame_;
    float frame_time_;
    bool expiring_;
    Gfx::ImageAnimPtr imgani_;
};

#endif  // INCLUDED_THOUSANDPOINTS
