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
#include "pngfile.h"
#include "../util/throw.h"

#include <png.h>
#include <sstream>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const int NUM_HEADER_BYTES_READ = 8;
}

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
PngFile::PngFile()
: width_(0)
, height_(0)
, bpp_(0)
{
}

///////////////////////////////////////////////////////////////////////////////
PngFile::~PngFile()
{
    Unload();
}

///////////////////////////////////////////////////////////////////////////////
void PngFile::Load(const std::string& filename, bool flip, bool reorder_rgb)
{
    LOG("Loading PNG file [" << filename << "]");

    Unload();
    filename_ = filename;

    FILE* file;
    if(fopen_s(&file, filename.c_str(), "rb") != 0)
    {
        THROW("Couldn't open the file [" << filename << "]");
    }

    png_byte header[NUM_HEADER_BYTES_READ];
    fread(header, 1, NUM_HEADER_BYTES_READ, file);
    if(png_sig_cmp(header, 0, NUM_HEADER_BYTES_READ) != 0)
    {
        fclose(file);
        THROW("The file [" << filename << "] is not a valid PNG file");
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    if(png_ptr == NULL)
    {
        fclose(file);
        THROW("Couldn't allocate a PNG structure while parsing the file [" << filename << "]");
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(file);
        THROW("Couldn't allocate a PNG structure while parsing the file [" << filename << "]");
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        fclose(file);
        THROW("Couldn't load the PNG file [" << filename << "]");
    }

    png_init_io(png_ptr, file);
    png_set_sig_bytes(png_ptr, NUM_HEADER_BYTES_READ);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR, NULL);

    width_  = png_get_image_width(png_ptr, info_ptr);
    height_ = png_get_image_height(png_ptr, info_ptr);
    bpp_    = png_get_channels(png_ptr, info_ptr) * png_get_bit_depth(png_ptr, info_ptr);

    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

    pixels_.reset(new unsigned char[width_ * height_ * (bpp_ >> 3)]);
    unsigned char* dst = pixels_.get();
    int num_row_bytes = width_ * (bpp_ >> 3);
    for(int i = 0; i < height_; i++)
    {
        memcpy(dst, row_pointers[i], num_row_bytes);
        dst += num_row_bytes;
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(file);

    if(flip)
    {
        Flip();
    }
    if(reorder_rgb)
    {
        ReorderRgb();
    }
}

///////////////////////////////////////////////////////////////////////////////
void PngFile::Save(const std::string& filename, int w, int h, int bpp, boost::shared_array<unsigned char> pixels)
{
}

///////////////////////////////////////////////////////////////////////////////
void PngFile::Unload()
{
    if(!filename_.empty())
    {
        LOG("Unloading PNG file [" << filename_ << "]");

        width_ = height_ = bpp_ = 0;
        pixels_.reset();
        filename_.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////
void PngFile::SetTransparentAlpha(unsigned char trans_r, unsigned char trans_g, unsigned char trans_b, bool reorder_rgb)
{
    LOG("Setting PNG file's alpha channel to transparent for color [" << int(trans_r) << ", " << int(trans_g) << ", " << int(trans_b) << "]");

    unsigned char *temp = new unsigned char[width_*height_*4];

    for(int y = 0; y < height_; y++)
    {
        for(int x = 0; x < width_; x++)
        {
            int offset = y * width_ + x;

            if(reorder_rgb)
            {
                temp[(offset * 4) + 0] = pixels_[(offset * 3) + 2];
                temp[(offset * 4) + 1] = pixels_[(offset * 3) + 1];
                temp[(offset * 4) + 2] = pixels_[(offset * 3) + 0];
            }
            else
            {
                temp[(offset * 4) + 0] = pixels_[(offset * 3) + 0];
                temp[(offset * 4) + 1] = pixels_[(offset * 3) + 1];
                temp[(offset * 4) + 2] = pixels_[(offset * 3) + 2];
            }

            if(trans_b == pixels_[(offset * 3) + 0] && trans_g == pixels_[(offset * 3) + 1] && trans_r  == pixels_[(offset * 3) + 2])
            {
                temp[(offset * 4) + 3] = (unsigned char)0;
            }
            else
            {
                temp[(offset * 4) + 3] = (unsigned char)0xFF;
            }
        }
    }

    pixels_.reset(temp);
    bpp_ = 32;
}

///////////////////////////////////////////////////////////////////////////////
void PngFile::SetOpaqueAlpha(bool reorder_rgb)
{
    LOG("Setting PNG file's alpha channel to opaque");

    unsigned char *temp = new unsigned char[width_*height_*4];

    for(int y = 0; y < height_; y++)
    {
        for(int x = 0; x < width_; x++)
        {
            int offset = y * width_ + x;

            if(reorder_rgb)
            {
                temp[(offset * 4) + 0] = pixels_[(offset * 3) + 2];
                temp[(offset * 4) + 1] = pixels_[(offset * 3) + 1];
                temp[(offset * 4) + 2] = pixels_[(offset * 3) + 0];
            }
            else
            {
                temp[(offset * 4) + 0] = pixels_[(offset * 3) + 0];
                temp[(offset * 4) + 1] = pixels_[(offset * 3) + 1];
                temp[(offset * 4) + 2] = pixels_[(offset * 3) + 2];
            }

            temp[(offset * 4) + 3] = (unsigned char)0xFF;
        }
    }

    pixels_.reset(temp);
    bpp_ = 32;
}

///////////////////////////////////////////////////////////////////////////////
void PngFile::Flip()
{
    LOG("Flipping pixel data for PNG file [" << filename_ << "]");

    // calc how many bytes per scanline
    long bytes_per_scanline = width_ * (bpp_ >> 3);

    unsigned char *temp = new unsigned char[height_ * bytes_per_scanline];

    // copy each line
    for(int row_index = 0; row_index < height_; row_index++)
    {
        memcpy(&temp[((height_ - 1) - row_index) * bytes_per_scanline]
              , &pixels_[row_index * bytes_per_scanline]
              , bytes_per_scanline);
    }

    // store the final pointer
    pixels_.reset(temp);
}

///////////////////////////////////////////////////////////////////////////////
void PngFile::ReorderRgb()
{
    LOG("Re-ordering pixel data for PNG file [" << filename_ << "]");

    int multiplier = bpp_ >> 3;
    unsigned char *temp = new unsigned char[width_*height_*multiplier];

    for(int y = 0; y < height_; y++)
    {
        for(int x = 0; x < width_; x++)
        {
            int offset = y * width_ + x;

            temp[(offset * multiplier) + 0] = pixels_[(offset * multiplier) + 2];
            temp[(offset * multiplier) + 1] = pixels_[(offset * multiplier) + 1];
            temp[(offset * multiplier) + 2] = pixels_[(offset * multiplier) + 0];
        }
    }

    pixels_.reset(temp);
}
