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
#ifndef INCLUDED_FUNKYSHOTENTITY
#define INCLUDED_FUNKYSHOTENTITY

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

struct IWorld;

///////////////////////////////////////////////////////////////////////////////
class FunkyShotEntity
    : public Gfx::IEntity
{
public:
    FunkyShotEntity(unsigned int num_remaining, IWorld* world, bool specify = false, float angle = 0.0f);

    void BindResources(Util::ResourceContext* resources);

    void Think(float time_delta);
    void Move(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    float BbWidth() const       { return 20.0f; }
    float BbHeight() const      { return 30.0f; }
    float BbHalfWidth() const   { return 10.0f; }
    float BbHalfHeight() const  { return 15.0f; }

private:
    IWorld*                     world_;
    std::vector<Aud::SoundPtr>  snd_explosions_;

    Gfx::ParticleImageAnim  bomb_;
    float blast_radius_;
    float blast_radius_squared_;
    float angle_;
    bool specify_;
    unsigned int num_remaining_;
};

#endif  // INCLUDED_FUNKYSHOTENTITY
