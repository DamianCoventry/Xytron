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
#include "font.h"
#include "fontfile.h"
#include "texture.h"
#include "color.h"
#include "../math/vector.h"
#include "../util/throw.h"

#include <gl/gl.h>
#include <gl/glext.h>

extern PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;


///////////////////////////////////////////////////////////////////////////////
namespace
{
    const char FIRST_CHAR   = 32;
    const int NUM_CHARS     = 96;
}

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
FontPtr Font::CreateFromFile(const std::string& filename)
{
    LOG("Creating a font from file [" << filename << "]");

    FontFile file;
    file.Load(filename);

    FontPtr font(new Font);
    font->Create(file.FaceName(), file.Rects(), file.SwatchSize(), file.Pixels());
    return font;
}





///////////////////////////////////////////////////////////////////////////////
Font::Font()
: swatch_size_(0)
{
}

///////////////////////////////////////////////////////////////////////////////
void Font::Create(const std::string& face_name, boost::shared_array<RECT> rects, int swatch_size, boost::shared_array<unsigned char> pixels)
{
    LOG("Creating font [" << face_name << "]");

    face_name_      = face_name;
    swatch_size_    = swatch_size;

    RECT* rect_ptr = rects.get();

    char_info_.clear();
    for(int i = 0; i < NUM_CHARS; i++)
    {
        CharInfo c;
        c.u0_   = float(rect_ptr[i].left) / float(swatch_size_);
        c.v0_   = float(rect_ptr[i].top) / float(swatch_size_);
        c.u1_   = float(rect_ptr[i].right) / float(swatch_size_);
        c.v1_   = float(rect_ptr[i].bottom) / float(swatch_size_);
        c.w_    = float(rect_ptr[i].right - rect_ptr[i].left);
        c.h_    = float(rect_ptr[i].bottom - rect_ptr[i].top);
        char_info_.push_back(c);
    }

    boost::shared_array<unsigned char> new_pixels(new unsigned char[swatch_size_*swatch_size_*4]);

    unsigned char trans_r = pixels[0];
    unsigned char trans_g = pixels[1];
    unsigned char trans_b = pixels[2];

    // Give the font an alpha channel
    for(int y = 0; y < swatch_size_; y++)
    {
        for(int x = 0; x < swatch_size_; x++)
        {
            int new_offset  = (y*swatch_size_+x)*4;
            int orig_offset = (y*swatch_size_+x)*3;

            new_pixels[new_offset+0] = pixels[orig_offset+0];
            new_pixels[new_offset+1] = pixels[orig_offset+1];
            new_pixels[new_offset+2] = pixels[orig_offset+2];

            if(trans_r == pixels[orig_offset+0] && trans_g == pixels[orig_offset+1] && trans_b == pixels[orig_offset+2])
            {
                new_pixels[new_offset+3] = 0;
            }
            else
            {
                new_pixels[new_offset+3] = 255;
            }
        }
    }

    surface_.reset(new Texture);
    surface_->CreateMipMap(swatch_size_, swatch_size_, 32, new_pixels);
}

///////////////////////////////////////////////////////////////////////////////
void Font::Delete()
{
    if(surface_)
    {
        LOG("Deleting font [" << face_name_ << "]");
        surface_.reset();
        face_name_.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Font::DrawString(const Math::Vector& position, const Color& color, const std::string& text)
{
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);

    glColor4f(color.r_, color.g_, color.b_, 1.0f);

    std::string::const_iterator c;
    int index;
    float x = position.x_;
    CharInfo* char_info;

    glBindTexture(GL_TEXTURE_2D, surface_->Id());
    for(c = text.begin(); c != text.end(); ++c)
    {
        index = *c - FIRST_CHAR;
        if(index >= 0 && index < NUM_CHARS)
        {
            char_info = &char_info_[index];
            glBegin(GL_TRIANGLE_STRIP);

                glTexCoord2f(char_info->u0_, char_info->v0_); glVertex2f(x, position.y_);
                glTexCoord2f(char_info->u0_, char_info->v1_); glVertex2f(x, position.y_+char_info->h_);
                glTexCoord2f(char_info->u1_, char_info->v0_); glVertex2f(x+char_info->w_, position.y_);
                glTexCoord2f(char_info->u1_, char_info->v1_); glVertex2f(x+char_info->w_, position.y_+char_info->h_);

            glEnd();

            x += char_info->w_;
        }
        else
        {
            x += 10.0f;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
float Font::PixelWidth(const std::string& text) const
{
    float pixel_width = 0;
    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); ++c)
    {
        int index = *c - FIRST_CHAR;
        if(index >= 0 && index < NUM_CHARS)
        {
            pixel_width += char_info_[*c - FIRST_CHAR].w_;
        }
        else
        {
            pixel_width += 10.0f;
        }
    }
    return pixel_width;
}

///////////////////////////////////////////////////////////////////////////////
int Font::NumChars(const std::string& text, float pixel_width) const
{
    float width = 0;
    int num_chars = 0;
    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); ++c)
    {
        int index = *c - FIRST_CHAR;
        if(index >= 0 && index < NUM_CHARS)
        {
            width += char_info_[index].w_;
        }
        else
        {
            width += 10.0f;
        }
        if(width > pixel_width) break;
        num_chars++;
    }
    return num_chars;
}
