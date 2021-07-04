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
#include "particle.h"

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
Particle::Particle()
: dead_(false)
, rotation_(0.0f)
, speed_(0.0f)
, speed_x_(0.0f)
, speed_y_(0.0f)
, alpha_(1.0f)
, alpha_inc_(0.0f)
, alpha_time_(0.0f)
, alpha_threshold_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void Particle::ThinkAlpha(float time_delta)
{
    if(alpha_ > 0.0f && alpha_ <= 1.0f)
    {
        alpha_time_ += time_delta;
        if(alpha_time_ >= alpha_threshold_)
        {
            alpha_time_ = 0.0f;
            alpha_ += alpha_inc_;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Particle::Move(float time_delta)
{
    position_.x_ += (speed_*time_delta) * -sinf(rotation_*Math::DTOR);
    position_.y_ += (speed_*time_delta) * cosf(rotation_*Math::DTOR);
}



///////////////////////////////////////////////////////////////////////////////
ParticlePixel::ParticlePixel()
: color_rotate_time_(0.0f)
, color_rotate_threshold_(0.0f) 
{
}

///////////////////////////////////////////////////////////////////////////////
void ParticlePixel::ThinkColor(float time_delta)
{
    if(!ColorFinished())
    {
        color_rotate_time_ += time_delta;
        if(color_rotate_time_ >= color_rotate_threshold_)
        {
            color_rotate_time_ = 0.0f;

            if(color_.r_ != color_dest_.r_)
            {
                color_.r_ += color_inc_.r_;
                if(color_.r_ > color_dest_.r_ - fabs(color_inc_.r_) && color_.r_ < color_dest_.r_ + fabs(color_inc_.r_))
                {
                    color_.r_ = color_dest_.r_;
                }
            }
            if(color_.g_ != color_dest_.g_)
            {
                color_.g_ += color_inc_.g_;
                if(color_.g_ > color_dest_.g_ - fabs(color_inc_.g_) && color_.g_ < color_dest_.g_ + fabs(color_inc_.g_))
                {
                    color_.g_ = color_dest_.g_;
                }
            }
            if(color_.b_ != color_dest_.b_)
            {
                color_.b_ += color_inc_.b_;
                if(color_.b_ > color_dest_.b_ - fabs(color_inc_.b_) && color_.b_ < color_dest_.b_ + fabs(color_inc_.b_))
                {
                    color_.b_ = color_dest_.b_;
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool ParticlePixel::ColorFinished() const
{
    return (color_.r_ == color_dest_.r_) && (color_.g_ == color_dest_.g_) && (color_.b_ == color_dest_.b_);
}



///////////////////////////////////////////////////////////////////////////////
ParticleImageAnim::ParticleImageAnim()
: current_frame_(0)
, imgani_time_(0.0f)
, imgani_threshold_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
void ParticleImageAnim::ThinkImageAnim(float time_delta)
{
    imgani_time_ += time_delta;
    if(imgani_time_ >= imgani_threshold_)
    {
        imgani_time_ = 0.0f;

        if(++current_frame_ >= imgani_->ImageCount())
        {
            current_frame_ = 0;
        }
    }
}
