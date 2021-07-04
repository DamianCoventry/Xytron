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
#ifndef INCLUDED_GFX_BITMAPFILE
#define INCLUDED_GFX_BITMAPFILE

#include <string>
#include <boost/shared_array.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
class BitmapFile
{
public:
    BitmapFile();
    ~BitmapFile();

    void Load(const std::string& filename, bool reorder_rgb = false, bool flip = false);
    void LoadGreyscale(const std::string& filename);
    void Save(const std::string& filename, int w, int h, int bpp, boost::shared_array<unsigned char> pixels);

    void Unload();

    void SetTransparentAlpha(bool reorder_rgb = true);
    void SetTransparentAlpha(unsigned char trans_r, unsigned char trans_g, unsigned char trans_b, bool reorder_rgb = true);
    void SetOpaqueAlpha(bool reorder_rgb = true);

    int GetWidth() const    { return info_header_.biWidth; }
    int GetHeight() const   { return info_header_.biHeight; }
    int GetBpp() const      { return info_header_.biBitCount; }

    boost::shared_array<unsigned char> Pixels() const { return pixels_; }

private:
    void ReorderRgb();
    void Flip();

private:
    std::string filename_;
    BITMAPFILEHEADER file_header_;
    BITMAPINFOHEADER info_header_;
    boost::shared_array<unsigned char> pixels_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_BITMAPFILE
