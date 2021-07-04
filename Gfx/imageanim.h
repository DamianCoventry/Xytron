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
#ifndef INCLUDED_GFX_IMAGEANIM
#define INCLUDED_GFX_IMAGEANIM

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <map>

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

class ImageAnim;
typedef boost::shared_ptr<ImageAnim> ImageAnimPtr;

class Texture;
typedef boost::shared_ptr<Texture> TexturePtr;

typedef std::map<std::string, ImageAnimPtr> ImageAnimList;

///////////////////////////////////////////////////////////////////////////////
class ImageAnim
{
public:
    static ImageAnimPtr CreateFromFile(const std::string& filename, int start_x, int start_y, int frame_width, int frame_height, int frame_count, bool trans = false, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
    static ImageAnimPtr CreateFromTexture(TexturePtr texture, int start_x, int start_y, int frame_width, int frame_height, int frame_count);

public:
    ~ImageAnim() { Delete(); }

    void Delete();

    void DrawCentered(bool centered);

    void Draw2d(const Graphics& g, const Math::Vector& position, float alpha = 1.0f);
    void Draw3d(const Graphics& g, float mv_matrix[16], const Math::Vector& position, float alpha = 1.0f);

    int FrameWidth() const           { return frame_w_; }
    int FrameHeight() const          { return frame_h_; }
    int ImageCount() const           { return int(images_.size()); }
    int CurrentFrame() const         { return current_frame_; }
    ImagePtr CurrentImage() const;
    TexturePtr Surface() const                  { return surface_; }
    ImagePtr Image(int current_frame) const     { return images_[current_frame]; }

    void IncCurrentFrame();
    void CurrentFrame(int current);

private:
    ImageAnim();

private:
    TexturePtr surface_;
    typedef std::vector<ImagePtr> Images;
    Images images_;
    int current_frame_;
    int frame_w_, frame_h_;
    bool draw_centered_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_IMAGEANIM
