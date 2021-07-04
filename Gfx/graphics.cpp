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
#include "graphics.h"
#include "texture.h"
#include "../math/vector.h"
#include "texcoord.h"
#include "../util/throw.h"

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glext.h>

using namespace Gfx;

PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

///////////////////////////////////////////////////////////////////////////////
Graphics::Graphics()
: window_(NULL)
, dc_(NULL)
, rc_(NULL)
, vfov_(60.0f)
, window_width_(0.0f)
, window_height_(0.0f)
, ncp_3d_(1.0f)
, fcp_3d_(1000000.0f)
, ncp_2d_(-10.0f)
, fcp_2d_(10.0f)
, clear_flags_(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT)
{
}

///////////////////////////////////////////////////////////////////////////////
bool Graphics::Initialise(HWND window, float window_width, float window_height, unsigned int bpp, unsigned int depth_bits)
{
    LOG("Initialising OpenGL [" << window_width << "x" << window_height << "], [" << bpp << " bpp], [" << depth_bits << " depth bits]");

    Shutdown();

    LOG("Retrieving a DC for the main window");
    dc_ = GetDC(window);
    if(dc_ == NULL)
    {
        THROW_WIN32("Couldn't get a DC for the given window");
    }

    PIXELFORMATDESCRIPTOR format;
    memset(&format, 0, sizeof(PIXELFORMATDESCRIPTOR));

    format.nSize         = sizeof(PIXELFORMATDESCRIPTOR);
    format.nVersion      = 1;
    format.dwFlags       = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    format.iPixelType    = PFD_TYPE_RGBA;
    format.cColorBits    = bpp;
    format.cDepthBits    = depth_bits;
    format.iLayerType    = PFD_MAIN_PLANE;

    int index = ChoosePixelFormat(dc_, &format);
    if(index == 0)
    {
        ReleaseDC(window_, dc_);
        THROW_WIN32("Couldn't choose a pixel format for the given window");
    }

    if(SetPixelFormat(dc_, index, &format) == FALSE)
    {
        ReleaseDC(window_, dc_);
        THROW_WIN32("Couldn't set the pixel format on the given window");
    }

    rc_ = wglCreateContext(dc_);
    if(rc_ == NULL)
    {
        ReleaseDC(window_, dc_);
        THROW_WIN32("Couldn't create an OpenGL rendering context");
    }

    if(wglMakeCurrent(dc_, rc_) == FALSE)
    {
        ReleaseDC(window_, dc_);
        THROW_WIN32("Couldn't make the rendering context current");
    }

    window_         = window;
    window_width_   = window_width;
    window_height_  = window_height;

    InitialiseMultitextureSupport();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    LOG("OpenGL initialised OK");

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::Shutdown()
{
    if(dc_)
    {
        LOG("Releasing the DC for the main window");
        ReleaseDC(window_, dc_);
        window_ = NULL;
        dc_     = NULL;
    }
    if(rc_)
    {
        LOG("Shutting down OpenGL");
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(rc_);
        rc_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::GetPerspectiveProjectionMatrix(double matrix[16])
{
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(vfov_, window_width_ / window_height_, ncp_3d_, fcp_3d_);

    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glPopMatrix();
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::Set3d()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(vfov_, window_width_ / window_height_, ncp_3d_, fcp_3d_);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::Set2d()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0f, window_width_, window_height_, 0.0f, ncp_2d_, fcp_2d_);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::SetClearFlags(bool clear_color, bool clear_depth)
{
    if(clear_color)
    {
        clear_flags_ |= GL_COLOR_BUFFER_BIT;
    }
    else
    {
        clear_flags_ &= ~GL_COLOR_BUFFER_BIT;
    }

    if(clear_depth)
    {
        clear_flags_ |= GL_DEPTH_BUFFER_BIT;
    }
    else
    {
        clear_flags_ &= ~GL_DEPTH_BUFFER_BIT;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::ClearBackBuffer()
{
    glClear(clear_flags_);
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::DrawColoredQuad(const Math::Vector& top_left, const Math::Vector& bot_right, const Color& color, float alpha) const
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    glColor4f(color.r_, color.g_, color.b_, alpha);
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(top_left.x_, top_left.y_);
        glVertex2f(top_left.x_, bot_right.y_);
        glVertex2f(bot_right.x_, top_left.y_);
        glVertex2f(bot_right.x_, bot_right.y_);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::Draw2dQuad(TexturePtr surface, const Math::Vector& position, float width, float height, const TexCoord& tl_tc, const TexCoord& br_tc, float alpha) const
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, surface->Id());

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    glBegin(GL_TRIANGLE_STRIP);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, tl_tc.u_, tl_tc.v_);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
        glVertex2f(position.x_, position.y_);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, tl_tc.u_, br_tc.v_);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
        glVertex2f(position.x_, position.y_+height);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, br_tc.u_, tl_tc.v_);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
        glVertex2f(position.x_+width, position.y_);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, br_tc.u_, br_tc.v_);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
        glVertex2f(position.x_+width, position.y_+height);
    glEnd();
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::Draw3dQuad(float mv_matrix[16], TexturePtr surface, const Math::Vector& position, float half_width, float half_height, const TexCoord& tl_tc, const TexCoord& br_tc, float alpha) const
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, surface->Id());

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    // Extract right and up vectors from the model-view matrix.
    Math::Vector right(mv_matrix[0], mv_matrix[4], mv_matrix[8]);
    Math::Vector up(mv_matrix[1], mv_matrix[5], mv_matrix[9]);

    // Scale the vectors to the size of our billboard image.
    right *= half_width;
    up *= half_height;

    // Create the four corners of our billboard image.
    Math::Vector top_left   = position - right + up;
    Math::Vector bot_left   = position - right - up;
    Math::Vector top_right  = position + right + up;
    Math::Vector bot_right  = position + right - up;

    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    glBegin(GL_TRIANGLE_STRIP);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, tl_tc.u_, tl_tc.v_);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
        glVertex3f(top_left.x_, top_left.y_, top_left.z_);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, tl_tc.u_, br_tc.v_);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
        glVertex3f(bot_left.x_, bot_left.y_, bot_left.z_);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, br_tc.u_, tl_tc.v_);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
        glVertex3f(top_right.x_, top_right.y_, top_right.z_);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, br_tc.u_, br_tc.v_);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
        glVertex3f(bot_right.x_, bot_right.y_, bot_right.z_);
    glEnd();
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::Draw2dPixelParticles(const ParticlePixelList& pixels) const
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_POINTS);
    ParticlePixelList::const_iterator itor;
    for(itor = pixels.begin(); itor != pixels.end(); ++itor)
    {
        if(!itor->dead_)
        {
            glColor4f(itor->color_.r_, itor->color_.g_, itor->color_.b_, itor->alpha_);
            glVertex2f(itor->position_.x_, itor->position_.y_);
        }
    }
    glEnd();
}

void Graphics::InitialiseMultitextureSupport()
{
    std::string Extensions((const char*)glGetString(GL_EXTENSIONS));

    if(Extensions.find("GL_ARB_multitexture") == std::string::npos)
    {
        throw std::runtime_error("Multi-texturing is not supported by the Display Adapter");
    }

	glMultiTexCoord1fARB	= (PFNGLMULTITEXCOORD1FARBPROC)		wglGetProcAddress("glMultiTexCoord1fARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
	glMultiTexCoord3fARB	= (PFNGLMULTITEXCOORD3FARBPROC)		wglGetProcAddress("glMultiTexCoord3fARB");
	glMultiTexCoord4fARB	= (PFNGLMULTITEXCOORD4FARBPROC)		wglGetProcAddress("glMultiTexCoord4fARB");
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC)	wglGetProcAddress("glClientActiveTextureARB");		
}
