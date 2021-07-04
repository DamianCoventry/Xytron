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
#ifndef INCLUDED_STATUSBARLEFT
#define INCLUDED_STATUSBARLEFT

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class StatusBarLeft
    : public Gfx::IEntity
{
public:
    StatusBarLeft(IWorld* world);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 10.0f; }
    float BbHeight() const      { return 10.0f; }
    float BbHalfWidth() const   { return 10.0f; }
    float BbHalfHeight() const  { return 10.0f; }

private:
    IWorld* world_;
    Gfx::ImagePtr img_background_;
    Gfx::ImagePtr img_playership_;
    Gfx::ImagePtr img_smallglasshighlight_;
    Gfx::ImagePtr img_roundedrecthighlight_;
    Gfx::ImagePtr img_hitpoints_;
    Gfx::ImagePtr img_hitpointshighlight_;
    Gfx::FontPtr fnt_hitpoints_;
    bool blink_;
    float blink_time_;
};

#endif  // INCLUDED_STATUSBARLEFT
