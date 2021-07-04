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
#ifndef INCLUDED_GFX_FONT
#define INCLUDED_GFX_FONT

#include <string>
#include <vector>
#include <map>

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
namespace Math
{
struct Vector;
}

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

struct Color;

class Texture;
typedef boost::shared_ptr<Texture> TexturePtr;

class Font;
typedef boost::shared_ptr<Font> FontPtr;

typedef std::map<std::string, FontPtr> FontList;

///////////////////////////////////////////////////////////////////////////////
class Font
{
public:
    static FontPtr CreateFromFile(const std::string& filename);
public:
    ~Font() { Delete(); }

    void Delete();
    void DrawString(const Math::Vector& position, const Color& color, const std::string& text);

    const std::string& FaceName() const { return face_name_; }

    float CharHeight() const { return char_info_[0].h_; }
    float PixelWidth(const std::string& text) const;
    int NumChars(const std::string& text, float pixel_width) const;

private:
    Font();
    void Create(const std::string& face_name, boost::shared_array<RECT> rects, int swatch_size, boost::shared_array<unsigned char> pixels);

private:
    std::string face_name_;
    int swatch_size_;
    TexturePtr surface_;
    struct CharInfo
    {
        float u0_, u1_, v0_, v1_, w_, h_;
    };
    typedef std::vector<CharInfo> CharInfoList;
    CharInfoList char_info_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_FONT
