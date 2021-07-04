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
#include "plasmashotexplosionentity.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float PRE_FADE_THRESHOLD  = 0.05f;
    const float ALPHA_THRESHOLD     = 0.0166666f;    // 60fps
}

///////////////////////////////////////////////////////////////////////////////
PlasmaShotExplosionEntity::PlasmaShotExplosionEntity(const Math::Vector& position, bool green)
: state_pre_fade_(true)
, pre_fade_time_(0.0f)
, num_sparks_(10)
, wake_size_(1.0f)
, green_(green)
{
    wake_.alpha_inc_       = -0.1f;
    wake_.alpha_threshold_ = ALPHA_THRESHOLD;
    wake_.position_        = position;

    int min_g = 220;
    int max_g = 255;
    int min_b = (green_ ?  80 : 220);
    int max_b = (green_ ? 100 : 255);

    for(int i = 0 ; i < num_sparks_; i++)
    {
        Gfx::ParticlePixel pixel;

        pixel.alpha_inc_                = -0.1f;
        pixel.alpha_threshold_          = 0.05f;
        pixel.speed_                    = float((rand()%100)+50);
        pixel.rotation_                 = float(rand()%360);
        pixel.position_                 = position;
        pixel.color_.r_                 = float((rand()%10)+10)/255.0f;
        pixel.color_.g_                 = float((rand()%(max_g-min_g))+min_g)/255.0f;
        pixel.color_.b_                 = float((rand()%(max_b-min_b))+min_b)/255.0f;
        pixel.color_dest_.r_            = 0.0f;
        pixel.color_dest_.g_            = 0.0f;
        pixel.color_dest_.b_            = 0.0f;
        pixel.color_inc_.r_             = -0.05f;
        pixel.color_inc_.g_             = -0.05f;
        pixel.color_inc_.b_             = -0.05f;
        pixel.color_rotate_threshold_   = 0.1f;

        sparks_.push_back(pixel);
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaShotExplosionEntity::BindResources(Util::ResourceContext* resources)
{
    wake_.image_ = resources->FindImage(green_ ? "Images/WakeGreen.tga" : "Images/WakeBlue.tga");
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaShotExplosionEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }

    wake_size_ += 200.0f * time_delta;

    if(state_pre_fade_)
    {
        pre_fade_time_ += time_delta;
        if(pre_fade_time_ >= PRE_FADE_THRESHOLD)
        {
            state_pre_fade_ = false;
        }
    }
    else
    {
        wake_.ThinkAlpha(time_delta);

        Gfx::ParticlePixelList::iterator itor_spark;
        for(itor_spark = sparks_.begin(); itor_spark != sparks_.end(); ++itor_spark)
        {
            if(itor_spark->dead_) continue;
            itor_spark->ThinkAlpha(time_delta);
            if(itor_spark->alpha_ <= 0.0f) itor_spark->dead_ = true;
            itor_spark->ThinkColor(time_delta);
            if(itor_spark->ColorFinished()) itor_spark->dead_ = true;
        }

        // Check what's dead.
        int spark_dead_count = 0;
        for(itor_spark = sparks_.begin(); itor_spark != sparks_.end(); ++itor_spark)
        {
            if(itor_spark->dead_) spark_dead_count++;
        }

        if(spark_dead_count == num_sparks_)
        {
            Kill();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaShotExplosionEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    Gfx::ParticlePixelList::iterator itor_spark;
    for(itor_spark = sparks_.begin(); itor_spark != sparks_.end(); ++itor_spark)
    {
        if(itor_spark->dead_) continue;
        itor_spark->Move(time_delta);
    }
}

///////////////////////////////////////////////////////////////////////////////
void PlasmaShotExplosionEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }

    g.Draw2dPixelParticles(sparks_);

    if(wake_.alpha_ > 0.0f)
    {
        Gfx::TexCoord tc0;
        Gfx::TexCoord tc1;
        tc0.u_ = tc0.v_ = 0.0f;
        tc1.u_ = tc1.v_ = 1.0f;

        g.Draw2dQuad(
            wake_.image_->Surface(),
            Position() - Math::Vector(wake_size_/2.0f, wake_size_/2.0f, 0.0f),
            wake_size_, wake_size_, tc0, tc1, wake_.alpha_);

        //wake_.image_->Draw2d(g, central_flash_.position_, wake_.alpha_);
    }
}
