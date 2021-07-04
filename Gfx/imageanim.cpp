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
#include "imageanim.h"
#include "image.h"
#include "texture.h"
#include "../math/vector.h"
#include "../util/throw.h"

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
ImageAnimPtr ImageAnim::CreateFromFile(const std::string& filename, int start_x, int start_y, int frame_width, int frame_height, int frame_count, bool trans, unsigned char r, unsigned char g, unsigned char b)
{
    LOG("Creating an image animation from file [" << filename << "]");

    TexturePtr texture = Texture::CreateFromFile(filename, trans, r, g, b);
    return ImageAnim::CreateFromTexture(texture, start_x, start_y, frame_width, frame_height, frame_count);
}

///////////////////////////////////////////////////////////////////////////////
ImageAnimPtr ImageAnim::CreateFromTexture(TexturePtr texture, int start_x, int start_y, int frame_width, int frame_height, int frame_count)
{
    LOG("Creating an image animation from the texture [" << texture->Id() << "]");

    ImageAnimPtr image_list(new ImageAnim);

    image_list->surface_ = texture;
    image_list->frame_w_ = frame_width;
    image_list->frame_h_ = frame_height;

    int x = start_x;
    int y = start_y;
    for(int i = 0; i < frame_count; i++)
    {
        if(x + frame_width > texture->Width())
        {
            x = start_x;
            if(y + frame_height > texture->Height())
            {
                // This amount of frames do not fit on this image
                return ImageAnimPtr();
            }
            y += frame_height;
        }

        ImagePtr image = Image::CreateFromTexture(texture, x, y, frame_width, frame_height);
        image_list->images_.push_back(image);

        x += frame_width;
    }

    return image_list;
}






///////////////////////////////////////////////////////////////////////////////
ImageAnim::ImageAnim()
: current_frame_(0)
, frame_w_(0)
, frame_h_(0)
, draw_centered_(true)
{
}

///////////////////////////////////////////////////////////////////////////////
void ImageAnim::Delete()
{
    surface_.reset();
    images_.clear();
    current_frame_ = 0;
}

///////////////////////////////////////////////////////////////////////////////
void ImageAnim::DrawCentered(bool centered)
{
    draw_centered_ = centered;
    Images::iterator itor;
    for(itor = images_.begin(); itor != images_.end(); ++itor)
    {
        (*itor)->DrawCentered(centered);
    }
}

///////////////////////////////////////////////////////////////////////////////
void ImageAnim::Draw2d(const Graphics& g, const Math::Vector& position, float alpha)
{
    if(current_frame_ >= 0 && current_frame_ < int(images_.size()))
    {
        images_[current_frame_]->Draw2d(g, position, alpha);
    }
}

///////////////////////////////////////////////////////////////////////////////
void ImageAnim::Draw3d(const Graphics& g, float mv_matrix[16], const Math::Vector& position, float alpha)
{
    if(current_frame_ >= 0 && current_frame_ < int(images_.size()))
    {
        images_[current_frame_]->Draw3d(g, mv_matrix, position, alpha);
    }
}

///////////////////////////////////////////////////////////////////////////////
ImagePtr ImageAnim::CurrentImage() const
{
    if(current_frame_ >= 0 && current_frame_ < int(images_.size()))
    {
        return images_[current_frame_];
    }
    return ImagePtr();
}

///////////////////////////////////////////////////////////////////////////////
void ImageAnim::IncCurrentFrame()
{
    ++current_frame_;
    if(current_frame_ >= int(images_.size()))
    {
        current_frame_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void ImageAnim::CurrentFrame(int current)
{
    current_frame_ = current;
    if(current_frame_ < 0)
    {
        current_frame_ = 0;
    }
    else if(current_frame_ > int(images_.size()) - 1)
    {
        current_frame_ = int(images_.size()) - 1;
    }
}
