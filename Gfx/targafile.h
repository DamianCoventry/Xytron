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
#ifndef INCLUDED_GFX_TARGAFILE
#define INCLUDED_GFX_TARGAFILE

#include <string>
#include <boost/shared_array.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
class TargaFile
{
public:
    TargaFile();
    ~TargaFile();

    void Load(const std::string& filename, bool flip = true, bool reorder_rgb = true);
    void Save(const std::string& filename, int w, int h, int bpp, boost::shared_array<unsigned char> pixels);
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

#endif  // INCLUDED_GFX_TARGAFILE
