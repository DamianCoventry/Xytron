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
#ifndef INCLUDED_GFX_TEXCOORD
#define INCLUDED_GFX_TEXCOORD

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

///////////////////////////////////////////////////////////////////////////////
struct TexCoord
{
    TexCoord() : u_(0.0f), v_(0.0f) {}
    TexCoord(float u, float v) : u_(u), v_(v) {}
    TexCoord(const TexCoord& tc) : u_(tc.u_), v_(tc.v_) {}
    float u_, v_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_TEXCOORD
