#ifndef INCLUDED_GFX_POLYGON
#define INCLUDED_GFX_POLYGON

#include "../math/vector.h"
#include "../math/Matrix.h"
#include "texcoord.h"
#include "texture.h"
#include <vector>

namespace Gfx
{

struct Polygon
{
    int Indices_[4];
    TexCoord TexCoords_[4];
    TexturePtr TexColor_;
    TexturePtr TexLightMap_;
    Math::Matrix Matrix_;
    float ULength_;
    float VLength_;
};

typedef std::vector<Polygon> PolygonList;

}       // namespace Gfx

#endif  // INCLUDED_GFX_POLYGON
