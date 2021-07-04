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
#include "railshotexplosionentity.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const float PRE_FADE_THRESHOLD  = 0.05f;
    const int NUM_BIG_SPARKS        = 6;
}

///////////////////////////////////////////////////////////////////////////////
RailShotExplosionEntity::RailShotExplosionEntity(const Math::Vector& position,
                                                   int num_sparks,
                                                   int min_r, int max_r,
                                                   int min_g, int max_g,
                                                   int min_b, int max_b)
: state_pre_fade_(true)
, pre_fade_time_(0.0f)
, num_sparks_(num_sparks)
{
    int i;
    for(i = 0 ; i < NUM_BIG_SPARKS; i++)
    {
        Gfx::ParticleImage image;

        image.alpha_inc_           = -0.05f;
        image.alpha_threshold_     = 0.05f;
        image.speed_               = float((rand()%150)+50);
        image.rotation_            = float(rand()%360);
        image.position_            = position;

        big_sparks_.push_back(image);
    }

    for(i = 0 ; i < num_sparks; i++)
    {
        Gfx::ParticlePixel pixel;

        pixel.alpha_inc_           = -0.1f;
        pixel.alpha_threshold_     = 0.05f;
        pixel.speed_               = float((rand()%100)+50);
        pixel.rotation_            = float(rand()%360);
        pixel.position_            = position;
        pixel.color_.r_                 = float((rand()%(max_r-min_r))+min_r)/255.0f;
        pixel.color_.g_                 = float((rand()%(max_g-min_g))+min_g)/255.0f;
        pixel.color_.b_                 = float((rand()%(max_b-min_b))+min_b)/255.0f;
        pixel.color_dest_.r_            = 0.0f;
        pixel.color_dest_.g_            = 0.0f;
        pixel.color_dest_.b_            = 0.0f;
        pixel.color_inc_.r_             = -0.05f;
        pixel.color_inc_.g_             = -0.05f;
        pixel.color_inc_.b_             = -0.05f;
        pixel.color_rotate_threshold_   = 0.1f;

        small_sparks_.push_back(pixel);
    }
}

///////////////////////////////////////////////////////////////////////////////
void RailShotExplosionEntity::BindResources(Util::ResourceContext* resources)
{
    Gfx::ParticleImageList::iterator itor;
    for(itor = big_sparks_.begin(); itor != big_sparks_.end(); ++itor)
    {
        itor->image_ = resources->FindImage(
            rand()%2 == 0 ? "Images/SparkSmall.tga" : "Images/SparkMedium.tga");
    }
}

///////////////////////////////////////////////////////////////////////////////
void RailShotExplosionEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }

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
        Gfx::ParticleImageList::iterator itor_big_spark;
        for(itor_big_spark = big_sparks_.begin(); itor_big_spark != big_sparks_.end(); ++itor_big_spark)
        {
            if(itor_big_spark->dead_) continue;
            itor_big_spark->ThinkAlpha(time_delta);
            if(itor_big_spark->alpha_ <= 0.0f) itor_big_spark->dead_ = true;
        }

        Gfx::ParticlePixelList::iterator itor_spark;
        for(itor_spark = small_sparks_.begin(); itor_spark != small_sparks_.end(); ++itor_spark)
        {
            if(itor_spark->dead_) continue;
            itor_spark->ThinkAlpha(time_delta);
            if(itor_spark->alpha_ <= 0.0f) itor_spark->dead_ = true;
            itor_spark->ThinkColor(time_delta);
            if(itor_spark->ColorFinished()) itor_spark->dead_ = true;
        }

        // Check what's dead.
        int big_spark_dead_count = 0;
        for(itor_big_spark = big_sparks_.begin(); itor_big_spark != big_sparks_.end(); ++itor_big_spark)
        {
            if(itor_big_spark->dead_) big_spark_dead_count++;
        }
        int spark_dead_count = 0;
        for(itor_spark = small_sparks_.begin(); itor_spark != small_sparks_.end(); ++itor_spark)
        {
            if(itor_spark->dead_) spark_dead_count++;
        }

        if(big_spark_dead_count == NUM_BIG_SPARKS && spark_dead_count == num_sparks_)
        {
            Kill();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void RailShotExplosionEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    Gfx::ParticleImageList::iterator itor_big_spark;
    for(itor_big_spark = big_sparks_.begin(); itor_big_spark != big_sparks_.end(); ++itor_big_spark)
    {
        if(itor_big_spark->dead_) continue;
        itor_big_spark->Move(time_delta);
    }
    Gfx::ParticlePixelList::iterator itor_spark;
    for(itor_spark = small_sparks_.begin(); itor_spark != small_sparks_.end(); ++itor_spark)
    {
        if(itor_spark->dead_) continue;
        itor_spark->Move(time_delta);
    }
}

///////////////////////////////////////////////////////////////////////////////
void RailShotExplosionEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }

    Gfx::ParticleImageList::iterator itor_big_spark;
    for(itor_big_spark = big_sparks_.begin(); itor_big_spark != big_sparks_.end(); ++itor_big_spark)
    {
        if(itor_big_spark->dead_) continue;
        itor_big_spark->image_->Draw2d(g, itor_big_spark->position_, itor_big_spark->alpha_);
    }

    g.Draw2dPixelParticles(small_sparks_);
}
