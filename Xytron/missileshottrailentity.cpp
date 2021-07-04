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
#include "stdafx.h"
#include "missileshottrailentity.h"
#include "iworld.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
MissileShotTrailEntity::MissileShotTrailEntity(IWorld* world, const Math::Vector& owner_position)
: world_(world)
, owner_position_(owner_position)
, emit_time_(0.0f)
, owner_death_(false)
{
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotTrailEntity::Think(float time_delta)
{
    if(Dead())
    {
        return;
    }

    if(!owner_death_)
    {
        emit_time_ += time_delta;
        if(emit_time_ >= 0.055f)
        {
            emit_time_ = 0.0f;

            std::ostringstream oss;
            oss << "Images/Smoke" << rand()%4 << ".tga";

            Gfx::ParticleImage image;
            image.position_         = owner_position_;
            image.alpha_inc_        = -0.1f;
            image.alpha_threshold_  = 0.05f;
            image.image_            = world_->ResourceContext()->FindImage(oss.str());
            images_.push_back(image);
            sizes_.push_back(1.0f);
        }
    }

    std::size_t dead_count = 0;
    Gfx::ParticleImageList::iterator itor_image;
    std::vector<float>::iterator itor_size;
    for(itor_image = images_.begin(), itor_size = sizes_.begin();
        itor_image != images_.end() && itor_size != sizes_.end();
        ++itor_image, ++itor_size)
    {
        if(itor_image->dead_)
        {
            dead_count++;
            continue;
        }

        itor_image->ThinkAlpha(time_delta);
        if(itor_image->alpha_ <= 0.0f) itor_image->dead_ = true;

        *itor_size += 150.0f * time_delta;
    }

    if(owner_death_ && (dead_count == images_.size()))
    {
        Kill();
    }
}

///////////////////////////////////////////////////////////////////////////////
void MissileShotTrailEntity::Draw2d(const Gfx::Graphics& g)
{
    if(Dead())
    {
        return;
    }

    Gfx::ParticleImageList::iterator itor_image;
    std::vector<float>::iterator itor_size;
    for(itor_image = images_.begin(), itor_size = sizes_.begin();
        itor_image != images_.end() && itor_size != sizes_.end();
        ++itor_image, ++itor_size)
    {
        if(itor_image->dead_) continue;

        Gfx::TexCoord tc0;
        Gfx::TexCoord tc1;
        tc0.u_ = tc0.v_ = 0.0f;
        tc1.u_ = tc1.v_ = 1.0f;

        g.Draw2dQuad(
            itor_image->image_->Surface(),
            itor_image->position_ - Math::Vector(*itor_size/2.0f, *itor_size/2.0f, 0.0f),
            *itor_size, *itor_size, tc0, tc1, itor_image->alpha_);

        //itor_image->image_->Draw2d(g, itor_image->position_, itor_image->alpha_);
    }
}
