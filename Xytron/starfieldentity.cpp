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
#include "starfieldentity.h"

///////////////////////////////////////////////////////////////////////////////
void StarfieldEntity::RandomiseStarsDirection()
{
    stars_.clear();

    float rotation = float(rand()%360);
    float s = -sinf(rotation*Math::DTOR);
    float c = cosf(rotation*Math::DTOR);

    for(int i = 0; i < 128; i++)
    {
        Gfx::ParticlePixel pixel;

        pixel.position_.x_ = 100.0f+float(rand()%600);
        pixel.position_.y_ = float(rand()%600);

        switch(rand()%5)
        {
        case 0:         // red
            pixel.color_.r_ = float((rand()%35)+220)/255.0f;
            pixel.color_.g_ = float((rand()%20)+5)/255.0f;
            pixel.color_.b_ = float((rand()%20)+5)/255.0f;
            break;
        case 1:         // green
            pixel.color_.r_ = float((rand()%20)+5)/255.0f;
            pixel.color_.g_ = float((rand()%35)+220)/255.0f;
            pixel.color_.b_ = float((rand()%20)+5)/255.0f;
            break;
        case 2:         // blue
            pixel.color_.r_ = float((rand()%20)+5)/255.0f;
            pixel.color_.g_ = float((rand()%20)+5)/255.0f;
            pixel.color_.b_ = float((rand()%35)+220)/255.0f;
            break;
        case 3:         // yellow
            pixel.color_.r_ = float((rand()%35)+220)/255.0f;
            pixel.color_.g_ = float((rand()%35)+220)/255.0f;
            pixel.color_.b_ = float((rand()%20)+5)/255.0f;
            break;
        case 4:         // orange
            pixel.color_.r_ = float((rand()%35)+220)/255.0f;
            pixel.color_.g_ = float((rand()%35)+110)/255.0f;
            pixel.color_.b_ = float((rand()%20)+5)/255.0f;
            break;
        }

        float speed = 10.0f + float(rand()%150);
        pixel.speed_x_ = speed * s;
        pixel.speed_y_ = speed * c;

        stars_.push_back(pixel);
    }
}

///////////////////////////////////////////////////////////////////////////////
void StarfieldEntity::Move(float time_delta)
{
    Gfx::ParticlePixelList::iterator itor_spark;
    for(itor_spark = stars_.begin(); itor_spark != stars_.end(); ++itor_spark)
    {
        itor_spark->position_.x_ += itor_spark->speed_x_*time_delta;
        if(itor_spark->position_.x_ < 100.0f)
        {
            itor_spark->position_.x_ += 600.0f;
        }
        else if(itor_spark->position_.x_ >= 600.0f)
        {
            itor_spark->position_.x_ -= 600.0f;
        }

        itor_spark->position_.y_ += itor_spark->speed_y_*time_delta;
        if(itor_spark->position_.y_ < 0.0f)
        {
            itor_spark->position_.y_ += 600.0f;
        }
        else if(itor_spark->position_.y_ >= 600.0f)
        {
            itor_spark->position_.y_ -= 600.0f;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void StarfieldEntity::Draw2d(const Gfx::Graphics& g)
{
    g.Draw2dPixelParticles(stars_);
}
