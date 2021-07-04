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
#ifndef INCLUDED_GFX_GRAPHICS
#define INCLUDED_GFX_GRAPHICS

#include <boost/shared_ptr.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include "particle.h"

///////////////////////////////////////////////////////////////////////////////
namespace Math
{
struct Vector;
}

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

struct TexCoord;

class Texture;
typedef boost::shared_ptr<Texture> TexturePtr;

///////////////////////////////////////////////////////////////////////////////
class Graphics
{
public:
    Graphics();
    ~Graphics() { Shutdown(); }

    bool Initialise(HWND window, float window_width, float window_height, unsigned int bpp, unsigned int depth_bits);
    void Shutdown();

    void Resize(float window_width, float window_height) { window_width_ = window_width; window_height_ = window_height; }

    void GetPerspectiveProjectionMatrix(double matrix[16]);

    void Vfov(float vfov)       { vfov_ = vfov; }
    void Ncp3d(float ncp_3d)    { ncp_3d_ = ncp_3d; }
    void Fcp3d(float fcp_3d)    { fcp_3d_ = fcp_3d; }
    void Ncp2d(float ncp_2d)    { ncp_2d_ = ncp_2d; }
    void Fcp2d(float fcp_2d)    { fcp_2d_ = fcp_2d; }

    float Vfov() const      { return vfov_; }
    float Ncp3d() const     { return ncp_3d_; }
    float Fcp3d() const     { return fcp_3d_; }
    float Ncp2d() const     { return ncp_2d_; }
    float Fcp2d() const     { return fcp_2d_; }

    void SetClearFlags(bool clear_color, bool clear_depth);
    void ClearBackBuffer();
    void Set2d();
    void Set3d();

    void PageFlip() { ::SwapBuffers(dc_); }

    void DrawColoredQuad(const Math::Vector& top_left, const Math::Vector& bot_right, const Color& color, float alpha = 1.0f) const;

    void Draw2dQuad(TexturePtr surface, const Math::Vector& position, float width, float height, const TexCoord& tl_tc, const TexCoord& br_tc, float alpha = 1.0f) const;
    void Draw3dQuad(float mv_matrix[16], TexturePtr surface, const Math::Vector& position, float half_width, float half_height, const TexCoord& tl_tc, const TexCoord& br_tc, float alpha = 1.0f) const;

    void Draw2dPixelParticles(const ParticlePixelList& pixels) const;

private:
    void InitialiseMultitextureSupport();

private:
    HWND    window_;
    HDC     dc_;
    HGLRC   rc_;

    unsigned int clear_flags_;

    float vfov_;
    float window_width_, window_height_;
    float ncp_3d_, fcp_3d_;
    float ncp_2d_, fcp_2d_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_GRAPHICS
