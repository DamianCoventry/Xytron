#ifndef INCLUDED_TEXTURESETDOCUMENT
#define INCLUDED_TEXTURESETDOCUMENT

#include <string>
#include <map>

#include "../gfx/color.h"

class TextureSetDocument
{
public:
    const std::string& ResCxt() const          { return res_cxt_; }

    const std::string& TexWall() const          { return tex_wall_; }
    const std::string& TexCeiling() const       { return tex_ceiling_; }
    const std::string& TexFloor() const         { return tex_floor_; }
    const std::string& TexLiquid() const        { return tex_liquid_; }
    const std::string& TexDoor() const          { return tex_door_; }
    const std::string& TexTransWall() const     { return tex_trans_wall_; }
    const std::string& TexTransDoor() const     { return tex_trans_door_; }
    const Gfx::Color& LiquidColor() const       { return liquid_color_; }

    void ResCxt(const std::string& res_cxt)                 { res_cxt_ = res_cxt; }

    void TexWall(const std::string& tex_wall)               { tex_wall_ = tex_wall; }
    void TexCeiling(const std::string& tex_ceiling)         { tex_ceiling_ = tex_ceiling; }
    void TexFloor(const std::string& tex_floor)             { tex_floor_ = tex_floor; }
    void TexLiquid(const std::string& tex_liquid)           { tex_liquid_ = tex_liquid; }
    void TexDoor(const std::string& tex_door)               { tex_door_ = tex_door; }
    void TexTransWall(const std::string& tex_trans_wall)    { tex_trans_wall_ = tex_trans_wall; }
    void TexTransDoor(const std::string& tex_trans_door)    { tex_trans_door_ = tex_trans_door; }
    void LiquidColor(const Gfx::Color& liquid_color)        { liquid_color_ = liquid_color; }

private:
    std::string res_cxt_;
    std::string tex_wall_;
    std::string tex_ceiling_;
    std::string tex_floor_;
    std::string tex_liquid_;
    std::string tex_door_;
    std::string tex_trans_wall_;
    std::string tex_trans_door_;
    Gfx::Color liquid_color_;
};

typedef std::map<std::string, TextureSetDocument> TextureSetDocumentMap;


#endif  // INCLUDED_TEXTURESETDOCUMENT
