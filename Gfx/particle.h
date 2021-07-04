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
#ifndef INCLUDED_GFX_PARTICLE
#define INCLUDED_GFX_PARTICLE

#include "../math/vector.h"
#include "color.h"
#include "image.h"
#include "imageanim.h"

#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
struct Particle
{
    Particle();

    void ThinkAlpha(float time_delta);
    void Move(float time_delta);

    bool dead_;

    Math::Vector position_;
    float rotation_;
    float speed_;
    float speed_x_;
    float speed_y_;

    float alpha_;
    float alpha_inc_;
    float alpha_time_;
    float alpha_threshold_;
};

///////////////////////////////////////////////////////////////////////////////
struct ParticlePixel
    : public Particle
{
    ParticlePixel();

    void ThinkColor(float time_delta);
    bool ColorFinished() const;

    Color color_;
    Color color_dest_;
    Color color_inc_;
    float color_rotate_time_;
    float color_rotate_threshold_;
};
typedef std::vector<ParticlePixel> ParticlePixelList;

///////////////////////////////////////////////////////////////////////////////
struct ParticleImage
    : public Particle
{
    ImagePtr image_;
};
typedef std::vector<ParticleImage> ParticleImageList;

///////////////////////////////////////////////////////////////////////////////
struct ParticleImageAnim
    : public Particle
{
    ParticleImageAnim();

    void ThinkImageAnim(float time_delta);

    ImageAnimPtr imgani_;
    int current_frame_;
    float imgani_time_;
    float imgani_threshold_;
};
typedef std::vector<ParticleImageAnim> ParticleImageAnimList;

}       // namespace Gfx

#endif  // INCLUDED_GFX_PARTICLE
