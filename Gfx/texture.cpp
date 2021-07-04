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
#include "texture.h"
#include "bitmapfile.h"
#include "targafile.h"
#include "pngfile.h"
#include "../util/throw.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <boost/algorithm/string.hpp>
#include <sstream>
#include <stdexcept>

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
TexturePtr Texture::CreateDefaultTexture()
{
    const int w = 128;
    const int h = 128;
    const int bpp = 32;

    boost::shared_array<unsigned char> pixels;
    pixels.reset(new unsigned char[w * h * (bpp >> 3)]);

    bool blah = false;
    unsigned char* ptr = pixels.get();
    for(int y = 0; y < h; y += 8)
    {
        for(int j = 0; j < 8; j++)
        {
            for(int x = 0; x < w; x += 16)
            {
                for(int i = 0; i < 8; i++)
                {
                    *ptr++ = (blah ? 255 : 0);
                    *ptr++ = (blah ? 255 : 0);
                    *ptr++ = (blah ? 255 : 0);
                    *ptr++ = 255;
                }
                for(int i = 0; i < 8; i++)
                {
                    *ptr++ = (blah ? 0 : 255);
                    *ptr++ = (blah ? 0 : 255);
                    *ptr++ = (blah ? 0 : 255);
                    *ptr++ = 255;
                }
            }
        }
        blah = !blah;
    }

    TexturePtr texture(new Texture);
    texture->CreateMipMap(w, h, bpp, pixels);
    return texture;
}

///////////////////////////////////////////////////////////////////////////////
TexturePtr Texture::CreateFromFile(const std::string& filename, bool trans, unsigned char r, unsigned char g, unsigned char b)
{
    LOG("Creating a texture from file [" << filename << "]");

    int w, h, bpp;
    boost::shared_array<unsigned char> pixels;

    std::string lwr_filename(boost::algorithm::to_lower_copy(filename));
    if(lwr_filename.find(".bmp") != std::string::npos)
    {
        BitmapFile file;
        file.Load(filename);
        if(trans)
        {
            file.SetTransparentAlpha(r, g, b);
        }
        w       = file.GetWidth();
        h       = file.GetHeight();
        bpp     = file.GetBpp();
        pixels  = file.Pixels();

    }
    else if(lwr_filename.find(".tga") != std::string::npos)
    {
        TargaFile file;
        file.Load(filename);
        w       = file.GetWidth();
        h       = file.GetHeight();
        bpp     = file.GetBpp();
        pixels  = file.Pixels();
    }
    else if(lwr_filename.find(".png") != std::string::npos)
    {
        PngFile file;
        file.Load(filename);
        if(trans)
        {
            file.SetTransparentAlpha(r, g, b);
        }
        w       = file.GetWidth();
        h       = file.GetHeight();
        bpp     = file.GetBpp();
        pixels  = file.Pixels();
    }
    else
    {
        THROW("The file [" << filename << "] has an unrecognised file extension");
    }

    TexturePtr texture(new Texture);
    texture->CreateMipMap(w, h, bpp, pixels);

    return texture;
}




///////////////////////////////////////////////////////////////////////////////
Texture::Texture()
: id_(0)
{
}

///////////////////////////////////////////////////////////////////////////////
void Texture::CreateMipMap(int width, int height, int bpp, boost::shared_array<unsigned char> pixels)
{
    Delete();

    glGenTextures(1, &id_);
    //CHECK_GL("Couldn't generate a new texture number");

    LOG("Building an OpenGL bi-linear mip mapped texture [" << width << "x" << height << ", " << bpp << " bpp] with the texture id [" << id_ << "]");

    glBindTexture(GL_TEXTURE_2D, id_);
    //CHECK_GL("Couldn't bind to texture [" << id_ << "]");
/*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //CHECK_GL("Couldn't set the texture magnification filter");
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //CHECK_GL("Couldn't set the texture minification filter");
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //CHECK_GL("Couldn't set the texture S wrapping parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //CHECK_GL("Couldn't set the texture T wrapping parameter");

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //CHECK_GL("Couldn't set the texture environment mode");
*/
    gluBuild2DMipmaps(GL_TEXTURE_2D, bpp>>3, width, height, bpp == 32 ? GL_RGBA : GL_BGR_EXT, GL_UNSIGNED_BYTE, (const GLvoid*)pixels.get());
    //CHECK_GL("Couldn't build a 2D mip mapped texture out of the pixel data");
}

void Texture::CreateBilinear(int width, int height, int bpp, boost::shared_array<unsigned char> pixels)
{
    Delete();

    glGenTextures(1, &id_);
    //CHECK_GL("Couldn't generate a new texture number");

    LOG("Building an OpenGL bi-linear texture [" << width << "x" << height << ", " << bpp << " bpp] with the texture id [" << id_ << "]");

    glBindTexture(GL_TEXTURE_2D, id_);
    //CHECK_GL("Couldn't bind to texture [" << id_ << "]");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //CHECK_GL("Couldn't set the texture S wrapping parameter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    //CHECK_GL("Couldn't set the texture T wrapping parameter");

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //CHECK_GL("Couldn't set the texture environment mode");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //CHECK_GL("Couldn't set the texture magnification filter");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //CHECK_GL("Couldn't set the texture minification filter");

    glTexImage2D(GL_TEXTURE_2D, 0, bpp >> 3, width, height, 0, bpp == 24 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)pixels.get());
    //CHECK_GL("Couldn't build a 2D texture out of the pixel data");
}

///////////////////////////////////////////////////////////////////////////////
void Texture::Delete()
{
    if(id_)
    {
        LOG("Deleting OpenGL texture [" << id_ << "]");
        glDeleteTextures(1, &id_);
        id_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
int Texture::Width() const
{
    int width;
    glBindTexture(GL_TEXTURE_2D, id_);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    return width;
}

///////////////////////////////////////////////////////////////////////////////
int Texture::Height() const
{
    int height;
    glBindTexture(GL_TEXTURE_2D, id_);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    return height;
}

///////////////////////////////////////////////////////////////////////////////
int Texture::Bpp() const
{
    int num_components;
    glBindTexture(GL_TEXTURE_2D, id_);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPONENTS, &num_components);
    return num_components * 8;
}
