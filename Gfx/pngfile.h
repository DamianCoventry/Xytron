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
#ifndef INCLUDED_GFX_PNGFILE
#define INCLUDED_GFX_PNGFILE

#include <string>
#include <boost/shared_array.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
class PngFile
{
public:
    PngFile();
    ~PngFile();

    void Load(const std::string& filename, bool flip = false, bool reorder_rgb = false);
    void Save(const std::string& filename, int w, int h, int bpp, boost::shared_array<unsigned char> pixels);

    void SetTransparentAlpha(unsigned char trans_r, unsigned char trans_g, unsigned char trans_b, bool reorder_rgb = true);
    void SetOpaqueAlpha(bool reorder_rgb = true);

    void Unload();

    int GetWidth() const    { return width_; }
    int GetHeight() const   { return height_; }
    int GetBpp() const      { return bpp_; }

    boost::shared_array<unsigned char> Pixels() const { return pixels_; }

private:
    void ReorderRgb();
    void Flip();

private:
    std::string filename_;
    int width_;
    int height_;
    int bpp_;
    boost::shared_array<unsigned char> pixels_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_PNGFILE
