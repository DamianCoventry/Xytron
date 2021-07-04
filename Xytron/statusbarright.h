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
#ifndef INCLUDED_STATUSBARRIGHT
#define INCLUDED_STATUSBARRIGHT

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class StatusBarRight
    : public Gfx::IEntity
{
public:
    StatusBarRight(IWorld* world);

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

    Gfx::ImageAnimPtr imgani_cannongun_;
    Gfx::ImageAnimPtr imgani_lasergun_;
    Gfx::ImageAnimPtr imgani_railgun_;
    Gfx::ImageAnimPtr imgani_plasmagun_;
    Gfx::ImageAnimPtr imgani_missilesgun_;
    Gfx::ImageAnimPtr imgani_gun_;

    Gfx::ImageAnimPtr imgani_funkybomb_;
    Gfx::ImageAnimPtr imgani_empbomb_;
    Gfx::ImageAnimPtr imgani_scatterbomb_;
    Gfx::ImageAnimPtr imgani_nukebomb_;
    Gfx::ImageAnimPtr imgani_bomb_;

    Gfx::ImageAnimPtr imgani_staticbombicons_;
    Gfx::ImagePtr img_largeglasshighlight_;
    Gfx::ImagePtr img_smallglasshighlight_;

    Gfx::ImagePtr img_cannongunoverlay_[4];
    Gfx::ImagePtr img_lasergunoverlay_[4];
    Gfx::ImagePtr img_missilesgunoverlay_[4];
    Gfx::ImagePtr img_railgunoverlay_[4];
    Gfx::ImagePtr img_plasmagunoverlay_[4];
    Gfx::ImagePtr img_gunoverlay_;

    Gfx::ImagePtr img_funkybomboverlay_;
    Gfx::ImagePtr img_empbomboverlay_;
    Gfx::ImagePtr img_nukebomboverlay_;
    Gfx::ImagePtr img_scatterbomboverlay_;
    Gfx::ImagePtr img_bomboverlay_;

    float frame_time_;
    int current_frame_;
};

#endif  // INCLUDED_STATUSBARRIGHT
