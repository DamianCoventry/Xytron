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
#ifndef INCLUDED_STARFIELDENTITY
#define INCLUDED_STARFIELDENTITY

#include "../gfx/gfx.h"

///////////////////////////////////////////////////////////////////////////////
class StarfieldEntity
    : public Gfx::IEntity
{
public:
    StarfieldEntity() { RandomiseStarsDirection(); }

    void RandomiseStarsDirection();

    virtual void Move(float time_delta);
    virtual void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 1.0f; }
    float BbHeight() const      { return 1.0f; }
    float BbHalfWidth() const   { return 0.5f; }
    float BbHalfHeight() const  { return 0.5f; }

protected:
    Gfx::ParticlePixelList stars_;
};

#endif  // INCLUDED_STARFIELDENTITY
