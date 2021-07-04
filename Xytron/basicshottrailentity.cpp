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
#include "basicshottrailentity.h"
#include "iworld.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
BasicShotTrailEntity::BasicShotTrailEntity(IWorld* world, const Math::Vector& owner_position)
: world_(world)
, owner_position_(owner_position)
, emit_time_(0.0f)
, owner_death_(false)
{
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotTrailEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }

    if(!owner_death_)
    {
        emit_time_ += time_delta;
        if(emit_time_ >= 0.02f)
        {
            emit_time_ = 0.0f;

            Gfx::ParticlePixel pixel;
            pixel.alpha_inc_                = -0.1f;
            pixel.alpha_threshold_          = 0.05f;
            pixel.position_                 = owner_position_;
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
            sparks_.push_back(pixel);
        }
    }

    std::size_t dead_count = 0;
    Gfx::ParticlePixelList::iterator itor_spark;
    for(itor_spark = sparks_.begin(); itor_spark != sparks_.end(); ++itor_spark)
    {
        if(itor_spark->dead_)
        {
            dead_count++;
            continue;
        }

        itor_spark->ThinkAlpha(time_delta);
        if(itor_spark->alpha_ <= 0.0f) itor_spark->dead_ = true;

        itor_spark->ThinkColor(time_delta);
        if(itor_spark->ColorFinished()) itor_spark->dead_ = true;
    }

    if(owner_death_ && (dead_count == sparks_.size()))
    {
        Kill();
    }
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotTrailEntity::Move(float time_delta)
{
    if(Dead())
    {
        return;
    }

    Gfx::ParticlePixelList::iterator itor_spark;
    for(itor_spark = sparks_.begin(); itor_spark != sparks_.end(); ++itor_spark)
    {
        if(itor_spark->dead_) continue;

        itor_spark->position_.x_ += float(rand()%3-1);
        itor_spark->position_.y_ += float(rand()%3-1);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BasicShotTrailEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }

    g.Draw2dPixelParticles(sparks_);
}
