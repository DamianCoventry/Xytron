#ifndef INCLUDED_WORLDTYPES
#define INCLUDED_WORLDTYPES

#include <map>
#include <string>

#include "../gfx/color.h"
#include "../gfx/texture.h"
#include "../gfx/polygon.h"
#include "../math/vector.h"
#include "../bsp/tree.h"

#define PIT_DEPTH       10

struct CellIndex
{
    bool operator<(const CellIndex& rhs) const
    {
        return (x_ < rhs.x_) || ((x_ == rhs.x_) && (z_ < rhs.z_));
    }

    bool operator==(const CellIndex& rhs) const
    {
        return (x_ == rhs.x_) && (z_ == rhs.z_);
    }

    CellIndex()
        : x_(0), z_(0) {}

    CellIndex(int x, int z)
        : x_(x), z_(z) {}

    int x_, z_;
};

enum ViewStyle
{
    VS_WIREFRAME = 0,
    VS_FRONTFACE_WIREFRAME,
    VS_COLORED_POLYGONS,
    VS_TEXTURED_POLYGONS
};

enum EditMode
{
    EM_CELL = 0,
    EM_TEXTURE_SET,
    EM_SOUND,
    EM_LIGHT,
    EM_ENTITY,
    EM_CAMERA,
    EM_REGION,
    EM_FOG,
    EM_TRIGGER,
    EM_SKY
};

enum CellType
{
    CT_SOLID_SPACE = 0,
    CT_NORMAL,
    CT_SECRET,
    CT_PIT,
    CT_LIQUID,
    CT_DOOR,
    CT_SECRET_DOOR,
    CT_TRANSLUCENT_DOOR,
    CT_TRANSLUCENT_SMALL_WALL,
    CT_TRANSLUCENT_LARGE_WALL
};

struct TextureSet
{
    Gfx::TexturePtr tex_wall_;
    Gfx::TexturePtr tex_ceiling_;
    Gfx::TexturePtr tex_floor_;
    Gfx::TexturePtr tex_liquid_;
    Gfx::TexturePtr tex_door_;
    Gfx::TexturePtr tex_trans_wall_;
    Gfx::TexturePtr tex_trans_door_;
    Gfx::Color liquid_color_;
    float tc_top_y_;
    float tc_y_[PIT_DEPTH+1];
    float tc_df_y_;
};
typedef std::map<std::string, TextureSet> TextureSetMap;

struct Cell
{
    Cell()
        : type_(CT_SOLID_SPACE)
        , center_x_(0.0f), center_z_(0.0f)
        , min_x_(0.0f), max_x_(0.0f)
        , min_z_(0.0f), max_z_(0.0f)
        , ts_(NULL)
        , tex_left_top_(NULL), tex_left_bot_(NULL)
        , tex_top_left_(NULL), tex_top_right_(NULL)
        , tex_right_top_(NULL), tex_right_bot_(NULL)
        , tex_bot_left_(NULL), tex_bot_right_(NULL)
        , door_state_(DS_CLOSED), sky_(false)
        , use_world_ambient_light_(true)
        , ambient_light_(25) {}

    CellType type_;
    std::string ts_name_;
    TextureSet* ts_;
    bool sky_;

    bool use_world_ambient_light_;
    int ambient_light_;             // 0-100

    Math::Vector door_offset_;
    enum DoorState { DS_CLOSED, DS_OPENED, DS_OPENING, DS_CLOSING };
    DoorState door_state_;

    float center_x_;
    float center_z_;
    float min_x_;
    float max_x_;
    float min_z_;
    float max_z_;

    TextureSet* tex_left_top_;
    TextureSet* tex_left_bot_;
    TextureSet* tex_top_left_;
    TextureSet* tex_top_right_;
    TextureSet* tex_right_top_;
    TextureSet* tex_right_bot_;
    TextureSet* tex_bot_left_;
    TextureSet* tex_bot_right_;

    Bsp::Tree bsp_tree_;          // The planes of this cell, and all of its neighbouring cells

    Gfx::PolygonList polygons_opaque_quads_;
    Gfx::PolygonList polygons_opaque_tris_;
    Gfx::PolygonList polygons_translucent_;
    Gfx::PolygonList polygons_door_;
    Gfx::PolygonList polygons_door_translucent_;
};
typedef std::map<CellIndex, Cell> CellContainer;

struct CellMap
{
    int AddPoint(const Math::Vector& V)
    {
        int I = int(Points_.size());
        Points_.push_back(V);
        return I;
    }

    Math::VectorList Points_;
    CellContainer Cells_;
};

enum CellBrush
{
    CB_SELECT = 0, 
    CB_SOLID_SPACE,
    CB_NORMAL,
    CB_SECRET,
    CB_PIT,
    CB_LIQUID,
    CB_DOOR,
    CB_SECRET_DOOR,
    CB_TRANSLUCENT_DOOR,
    CB_TRANSLUCENT_SMALL_WALL,
    CB_TRANSLUCENT_LARGE_WALL
};

enum TextureSetBrush
{
    TSB_SELECT = 0,
    TSB_APPLY
};

enum SkyBrush
{
    SB_SELECT = 0,
    SB_SETSKY,
    SB_CLEARSKY
};

enum TextureSetChannel
{
    TSC_WALL = 0,
    TSC_CEILING,
    TSC_FLOOR,
    TSC_LIQUID,
    TSC_DOOR,
    TSC_TRANS_WALL,
    TSC_TRANS_DOOR
};

enum LightTool
{
    LT_SELECTLIGHT,
    LT_CELLAMBIENCE,
    LT_LIGHTRADIUS
};

#endif  // INCLUDED_WORLDTYPES
