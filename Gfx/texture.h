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
#ifndef INCLUDED_GFX_TEXTURE
#define INCLUDED_GFX_TEXTURE

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <map>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

class Texture;
typedef boost::shared_ptr<Texture> TexturePtr;

typedef std::map<std::string, TexturePtr> TextureList;

///////////////////////////////////////////////////////////////////////////////
class Texture
{
public:
    static TexturePtr CreateDefaultTexture();
    static TexturePtr CreateFromFile(const std::string& filename, bool trans = false, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);

public:
    Texture();
    ~Texture() { Delete(); }

    void CreateMipMap(int width, int height, int bpp, boost::shared_array<unsigned char> pixels);
    void CreateBilinear(int width, int height, int bpp, boost::shared_array<unsigned char> pixels);
    void Delete();

    unsigned int Id() const { return id_; }
    int Width() const;
    int Height() const;
    int Bpp() const;

private:
    unsigned int id_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_TEXTURE
