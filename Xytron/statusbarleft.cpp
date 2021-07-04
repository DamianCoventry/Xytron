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
#include "statusbarleft.h"
#include "iworld.h"
#include "xytron.h"
#include "playerentity.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
StatusBarLeft::StatusBarLeft(IWorld* world)
: world_(world)
, blink_(false)
, blink_time_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void StatusBarLeft::BindResources(Util::ResourceContext* resources)
{
    img_background_             = resources->FindImage("Images/StatusBarLeft.tga");
    img_playership_             = resources->FindImage("Images/PlayerShipIcon.png");
    img_smallglasshighlight_    = resources->FindImage("Images/SmallGlassHighlight.tga");
    img_roundedrecthighlight_   = resources->FindImage("Images/RoundedRectHighlight.tga");
    img_hitpoints_              = resources->FindImage("Images/Hitpoints.tga");
    img_hitpointshighlight_     = resources->FindImage("Images/HitpointsHighlight.tga");

    fnt_hitpoints_ = resources->FindFont("Fonts/Impact18Regular.font");

    img_background_->DrawCentered(false);
    img_playership_->DrawCentered(false);
    img_smallglasshighlight_->DrawCentered(false);
    img_roundedrecthighlight_->DrawCentered(false);
    img_hitpoints_->DrawCentered(false);
    img_hitpointshighlight_->DrawCentered(false);
}

///////////////////////////////////////////////////////////////////////////////
void StatusBarLeft::Think(float time_delta)
{
    unsigned int percent = world_->Player()->HitPoints()*100/PlayerEntity::FULL_HITPOINTS;
    if(percent <= 33)
    {
        blink_time_ += time_delta;
        if(blink_time_ >= 0.25f)
        {
            blink_time_ = 0.0f;
            blink_ = !blink_;
        }
    }
    else
    {
        blink_time_ = 0.0f;
        blink_      = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void StatusBarLeft::Draw2d(const Gfx::Graphics& g)
{
    img_background_->Draw2d(g, Math::Vector());

    std::ostringstream oss;
    oss << (world_->GetWave()+1) << " of " << Xytron::MAX_WAVES;
    world_->GetFont()->DrawString(Math::Vector(20.0f, 34.0f, 0.0f), Gfx::Color(1.0f, 1.0f, 0.0f), oss.str());

    oss.str("");
    oss << world_->Player()->Score();
    world_->GetFont()->DrawString(Math::Vector(20.0f, 102.0f, 0.0f), Gfx::Color(1.0f, 1.0f, 0.0f), oss.str());

    unsigned int percent = world_->Player()->HitPoints()*100/PlayerEntity::FULL_HITPOINTS;
    oss.str("");
    oss << percent << "%";

    Gfx::Color color;
    if(percent >= 0 && percent <= 33)
    {
        color.r_ = 1.0f;            // red
    }
    else if(percent > 34 && percent <= 66)
    {
        color.g_ = 1.0f;            // green
    }
    else
    {
        color.r_ = color.g_ = 1.0f; // yellow
    }
    g.DrawColoredQuad(Math::Vector(3.0f, 280.0f-float(percent), 0.0f), Math::Vector(95.0f, 280.0f, 0.0f), color);
    img_hitpoints_->Draw2d(g, Math::Vector(1.0f, 178.0f, 0.0f));

    if(blink_)
    {
        float half_txt_w = fnt_hitpoints_->PixelWidth(oss.str())/2.0f;
        fnt_hitpoints_->DrawString(Math::Vector(53.0f-half_txt_w, 215.0f, 0.0f), color, oss.str());
    }

    float y;
    if(world_ && world_->Player() && world_->Player())
    {
        y = 317.0f;
        for(unsigned int i = 1; i < world_->Player()->RemainingShips(); i++)
        {
            img_playership_->Draw2d(g, Math::Vector(28.0f, y, 0.0f));
            y += 56.0f;
        }
    }

    img_roundedrecthighlight_->Draw2d(g, Math::Vector(5.0f, 24.0f, 0.0f));
    img_roundedrecthighlight_->Draw2d(g, Math::Vector(5.0f, 92.0f, 0.0f));
    img_hitpointshighlight_->Draw2d(g, Math::Vector(1.0f, 178.0f, 0.0f));

    y = 306.0f;
    for(int i = 0; i < 5; i++)
    {
        img_smallglasshighlight_->Draw2d(g, Math::Vector(18.0f, y, 0.0f));
        y += 56.0f;
    }
}
