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
#ifndef INCLUDED_GFX_VIEWPORT
#define INCLUDED_GFX_VIEWPORT

#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
class Viewport
{
public:
    Viewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
        : x_(x), y_(y), w_(w), h_(h) {}
    void Set();
private:
    unsigned int x_, y_;
    unsigned int w_, h_;
};

typedef std::vector<Viewport> ViewportList;

}       // namespace Gfx

#endif  // INCLUDED_GFX_VIEWPORT
