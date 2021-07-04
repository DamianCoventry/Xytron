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
#ifndef INCLUDED_GFX_FONTFILE
#define INCLUDED_GFX_FONTFILE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <boost/shared_array.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
class FontFile
{
public:
    static const int NUM_CHARS = 96;

public:
    FontFile();
    ~FontFile();

    void Load(const std::string& filename);

    const std::string& FaceName() const      { return face_name_; }
    int SwatchSize() const                   { return swatch_size_; }

    boost::shared_array<RECT> Rects() const              { return rects_; }
    boost::shared_array<unsigned char> Pixels() const    { return pixels_; }

private:
    std::string face_name_;
    int swatch_size_;
    boost::shared_array<RECT> rects_;
    boost::shared_array<unsigned char> pixels_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_FONTFILE
