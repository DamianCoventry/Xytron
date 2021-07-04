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
#include "scatterexplosionentity.h"
#include "basicshotentity.h"
#include "playerentity.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const int NUM_FIREBALLS     = 12;
    const int NUM_SMALL_SPARKS  = 50;
    const int NUM_EXPLOSIONS    = 4;

    const float ALPHA_THRESHOLD     = 0.0166666f;    // 60fps
    const float ALPHA_DEC_VALUE     = -0.1f;
    const float PRE_FADE_THRESHOLD  = 0.05f;
}

///////////////////////////////////////////////////////////////////////////////
ScatterExplosionEntity::ScatterExplosionEntity(const Math::Vector& position)
: state_pre_fade_(true)
, pre_fade_time_(0.0f)
{
    wake_size_ = 16.0f;
    central_flash_.alpha_inc_       = ALPHA_DEC_VALUE;
    central_flash_.alpha_threshold_ = ALPHA_THRESHOLD;
    central_flash_.position_        = position;

    wake_.alpha_inc_       = -0.1f;
    wake_.alpha_threshold_ = ALPHA_THRESHOLD;
    wake_.position_        = position;

    int i;
    for(i = 0 ; i < NUM_FIREBALLS; i++)
    {
        Gfx::ParticleImage image;

        image.alpha_inc_           = -0.1f;
        image.alpha_threshold_     = 0.05f;
        image.speed_               = float((rand()%100)+50);
        image.rotation_            = float(rand()%360);
        image.position_            = position;

        fireballs_.push_back(image);
    }

    for(i = 0 ; i < NUM_SMALL_SPARKS; i++)
    {
        Gfx::ParticlePixel pixel;

        pixel.alpha_inc_           = -0.1f;
        pixel.alpha_threshold_     = 0.05f;
        pixel.speed_               = float((rand()%200)+100);
        pixel.rotation_            = float(rand()%360);
        pixel.position_            = position;
        pixel.color_.r_                 = float((rand()%35)+220)/255.0f;
        pixel.color_.g_                 = float((rand()%35)+220)/255.0f;
        pixel.color_.b_                 = float((rand()%20)+5)/255.0f;
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
void ScatterExplosionEntity::BindResources(Util::ResourceContext* resources)
{
    std::ostringstream oss;
    oss << "Images/ExplosionMedium" << (rand()%NUM_EXPLOSIONS) << ".tga";
    central_flash_.image_ = resources->FindImage(oss.str());

    wake_.image_ = resources->FindImage("Images/WakeRed.tga");

    fireballs_[0].image_ = resources->FindImage("Images/Fireball0.tga");
    fireballs_[1].image_ = resources->FindImage("Images/Fireball1.tga");
    fireballs_[2].image_ = resources->FindImage("Images/Fireball2.tga");
    fireballs_[3].image_ = resources->FindImage("Images/Fireball3.tga");
    fireballs_[4].image_ = resources->FindImage("Images/Fireball4.tga");
    fireballs_[5].image_ = resources->FindImage("Images/Fireball5.tga");
    fireballs_[6].image_ = fireballs_[0].image_;
    fireballs_[7].image_ = fireballs_[1].image_;
    fireballs_[8].image_ = fireballs_[2].image_;
    fireballs_[9].image_ = fireballs_[3].image_;
    fireballs_[10].image_ = fireballs_[4].image_;
    fireballs_[11].image_ = fireballs_[5].image_;
}

///////////////////////////////////////////////////////////////////////////////
void ScatterExplosionEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }

    wake_size_ += 500.0f * time_delta;

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
        // Fade out the central flash image.
        central_flash_.ThinkAlpha(time_delta);
        wake_.ThinkAlpha(time_delta);

        Gfx::ParticleImageList::iterator itor_fireball;
        for(itor_fireball = fireballs_.begin(); itor_fireball != fireballs_.end(); ++itor_fireball)
        {
            if(itor_fireball->dead_) continue;
            itor_fireball->ThinkAlpha(time_delta);
            if(itor_fireball->alpha_ <= 0.0f) itor_fireball->dead_ = true;
        }

        Gfx::ParticlePixelList::iterator itor_small_spark;
        for(itor_small_spark = small_sparks_.begin(); itor_small_spark != small_sparks_.end(); ++itor_small_spark)
        {
            if(itor_small_spark->dead_) continue;
            itor_small_spark->ThinkAlpha(time_delta);
            if(itor_small_spark->alpha_ <= 0.0f) itor_small_spark->dead_ = true;
            itor_small_spark->ThinkColor(time_delta);
            if(itor_small_spark->ColorFinished()) itor_small_spark->dead_ = true;
        }

        // Check what's dead.
        int fireball_dead_count = 0;
        for(itor_fireball = fireballs_.begin(); itor_fireball != fireballs_.end(); ++itor_fireball)
        {
            if(itor_fireball->dead_) fireball_dead_count++;
        }
        int small_spark_dead_count = 0;
        for(itor_small_spark = small_sparks_.begin(); itor_small_spark != small_sparks_.end(); ++itor_small_spark)
        {
            if(itor_small_spark->dead_) small_spark_dead_count++;
        }

        if(fireball_dead_count == NUM_FIREBALLS && small_spark_dead_count == NUM_SMALL_SPARKS)
        {
            Kill();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void ScatterExplosionEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }
    Gfx::ParticleImageList::iterator itor_fireball;
    for(itor_fireball = fireballs_.begin(); itor_fireball != fireballs_.end(); ++itor_fireball)
    {
        if(itor_fireball->dead_) continue;
        itor_fireball->Move(time_delta);
    }
    Gfx::ParticlePixelList::iterator itor_small_spark;
    for(itor_small_spark = small_sparks_.begin(); itor_small_spark != small_sparks_.end(); ++itor_small_spark)
    {
        if(itor_small_spark->dead_) continue;
        itor_small_spark->Move(time_delta);
    }
}

///////////////////////////////////////////////////////////////////////////////
void ScatterExplosionEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }

    Gfx::ParticleImageList::iterator itor_fireball;
    for(itor_fireball = fireballs_.begin(); itor_fireball != fireballs_.end(); ++itor_fireball)
    {
        if(itor_fireball->dead_) continue;
        itor_fireball->image_->Draw2d(g, itor_fireball->position_, itor_fireball->alpha_);
    }

    g.Draw2dPixelParticles(small_sparks_);

    if(central_flash_.alpha_ > 0.0f)
    {
        central_flash_.image_->Draw2d(g, central_flash_.position_, central_flash_.alpha_);
    }

    if(wake_.alpha_ > 0.0f)
    {
        Gfx::TexCoord tc0;
        Gfx::TexCoord tc1;
        tc0.u_ = tc0.v_ = 0.0f;
        tc1.u_ = tc1.v_ = 1.0f;

        g.Draw2dQuad(
            wake_.image_->Surface(),
            central_flash_.position_ - Math::Vector(wake_size_/2.0f, wake_size_/2.0f, 0.0f),
            wake_size_, wake_size_, tc0, tc1, wake_.alpha_);

        //wake_.image_->Draw2d(g, central_flash_.position_, wake_.alpha_);
    }
}
