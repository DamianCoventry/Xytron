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
#include "image.h"
#include "texture.h"
#include "../math/vector.h"
#include "graphics.h"
#include "../util/throw.h"

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
ImagePtr Image::CreateFromFile(const std::string& filename, int start_x, int start_y, int frame_width, int frame_height, bool trans, unsigned char r, unsigned char g, unsigned char b)
{
    LOG("Creating an image from file [" << filename << "]");

    TexturePtr tex = Texture::CreateFromFile(filename, trans, r, g, b);
    return CreateFromTexture(tex, start_x, start_y, frame_width, frame_height);
}

///////////////////////////////////////////////////////////////////////////////
ImagePtr Image::CreateFromTexture(TexturePtr texture, int start_x, int start_y, int frame_width, int frame_height)
{
    LOG("Creating an image from the texture [" << texture->Id() << "]");

    ImagePtr image(new Image);

    image->surface_    = texture;
    image->frame_w_    = float(frame_width);
    image->frame_h_    = float(frame_height);

    image->frame_half_w_ = image->frame_w_ / 2.0f;
    image->frame_half_h_ = image->frame_h_ / 2.0f;

    int texture_width   = texture->Width();
    int texture_height  = texture->Height();

    image->tc0_.u_ = float(start_x) / float(texture_width);
    image->tc0_.v_ = float(start_y) / float(texture_height);
    image->tc1_.u_ = float(start_x+frame_width) / float(texture_width);
    image->tc1_.v_ = float(start_y+frame_height) / float(texture_height);

    return image;
}






///////////////////////////////////////////////////////////////////////////////
Image::Image()
: frame_w_(0.0f)
, frame_h_(0.0f)
, frame_half_w_(0.0f)
, frame_half_h_(0.0f)
, draw_centered_(true)
{
    tc0_.u_ = tc1_.u_ = 0.0f;
    tc0_.v_ = tc1_.v_ = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
void Image::Delete()
{
    surface_.reset();
}

///////////////////////////////////////////////////////////////////////////////
void Image::Draw2d(const Graphics& g, const Math::Vector& position, float alpha) const
{
    if(draw_centered_)
    {
        g.Draw2dQuad(surface_, position - Math::Vector(frame_half_w_, frame_half_h_, 0.0f), frame_w_, frame_h_, tc0_, tc1_, alpha);
    }
    else
    {
        g.Draw2dQuad(surface_, position, frame_w_, frame_h_, tc0_, tc1_, alpha);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Image::Draw3d(const Graphics& g, float mv_matrix[16], const Math::Vector& position, float alpha) const
{
    g.Draw3dQuad(mv_matrix, surface_, position, frame_half_w_, frame_half_h_, tc0_, tc1_, alpha);
}
