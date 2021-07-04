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
#ifndef INCLUDED_GFX_COLOR
#define INCLUDED_GFX_COLOR

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
struct Color
{
    Color(float c = 0.0f)
        : r_(c), g_(c), b_(c) {}
    Color(float r, float g, float b)
        : r_(r), g_(g), b_(b) {}
    float r_, g_, b_;
};

///////////////////////////////////////////////////////////////////////////////
struct Color2
{
    Color2(float c = 0.0f)
        : r_(c), g_(c), b_(c), a_(1.0f) {}
    Color2(float r, float g, float b)
        : r_(r), g_(g), b_(b), a_(1.0f) {}
    Color2(float r, float g, float b, float a)
        : r_(r), g_(g), b_(b), a_(a) {}
    float r_, g_, b_, a_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_COLOR
