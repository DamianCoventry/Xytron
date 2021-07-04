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
#ifndef INCLUDED_GFX_IMAGE
#define INCLUDED_GFX_IMAGE

#include <boost/shared_ptr.hpp>
#include <string>
#include <map>
#include "texcoord.h"

///////////////////////////////////////////////////////////////////////////////
namespace Math
{
struct Vector;
}

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

class Graphics;

class Image;
typedef boost::shared_ptr<Image> ImagePtr;

class Texture;
typedef boost::shared_ptr<Texture> TexturePtr;

typedef std::map<std::string, ImagePtr> ImageList;

///////////////////////////////////////////////////////////////////////////////
class Image
{
public:
    static ImagePtr CreateFromFile(const std::string& filename, int start_x, int start_y, int frame_width, int frame_height, bool trans = false, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
    static ImagePtr CreateFromTexture(TexturePtr texture, int start_x, int start_y, int frame_width, int frame_height);

public:
    ~Image() { Delete(); }

    void Delete();

    void DrawCentered(bool centered) { draw_centered_ = centered; }

    float FrameWidth() const         { return frame_w_; }
    float FrameHeight() const        { return frame_h_; }
    TexturePtr Surface() const       { return surface_; }
    const TexCoord& Tc0() const      { return tc0_; }
    const TexCoord& Tc1() const      { return tc1_; }

    void Draw2d(const Graphics& g, const Math::Vector& position, float alpha = 1.0f) const;
    void Draw3d(const Graphics& g, float mv_matrix[16], const Math::Vector& position, float alpha = 1.0f) const;

private:
    Image();

private:
    TexturePtr surface_;
    TexCoord tc0_;
    TexCoord tc1_;
    float frame_w_;
    float frame_h_;
    float frame_half_w_;
    float frame_half_h_;
    bool draw_centered_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_IMAGE
