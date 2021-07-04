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
#ifndef INCLUDED_GFX_DISPLAYMODE
#define INCLUDED_GFX_DISPLAYMODE

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
class DisplayMode
{
public:
    DisplayMode(unsigned int width, unsigned int height, unsigned int bpp, unsigned int hz)
        : width_(width), height_(height), bpp_(bpp), hz_(hz) {}
    unsigned int Width() const  { return width_; }
    unsigned int Height() const { return height_; }
    unsigned int Bpp() const    { return bpp_; }
    unsigned int Hz() const     { return hz_; }
    bool Set();
private:
    unsigned int width_;
    unsigned int height_;
    unsigned int bpp_;
    unsigned int hz_;
};

typedef std::vector<DisplayMode> DisplayModeList;



///////////////////////////////////////////////////////////////////////////////
struct DisplayModes
{
    static DisplayModeList Modes();
    static bool SetModeFromRegistry();
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_DISPLAYMODE
