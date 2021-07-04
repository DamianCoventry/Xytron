#include "cellmapdocument.h"
#include "../util/resourcecontext.h"
#include "../math/linesegment.h"
#include "../gfx/color.h"

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/glext.h>
#include <mmsystem.h>

extern PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

float CellMapDocument::DOOR_FRAME_HEIGHT            = 8.0f;
float CellMapDocument::DOOR_HALF_THICKNESS          = 16.0f;
float CellMapDocument::DOOR_FRAME_HALF_THICKNESS    = 24.0f;
float CellMapDocument::SKY_SCHOCHURE_HEIGHT         = 16.0f;
int CellMapDocument::LIGHT_MAP_SIZE                 = 8;

namespace
{
    Gfx::Color colors_[16];
    int current_color_ = 0;
}

CellMapDocument::CellMapDocument()
: cell_size_(128.0f)
, default_texture_set_name_("(DefaultTextureSet)")
, min_sky_x_(0.0f)
, max_sky_x_(0.0f)
, min_sky_z_(0.0f)
, max_sky_z_(0.0f)
, ambient_(25)
{
    colors_[0] = Gfx::Color(1.0f, 0.0f, 0.0f);
    colors_[1] = Gfx::Color(0.8f, 0.0f, 0.0f);
    colors_[2] = Gfx::Color(0.6f, 0.0f, 0.0f);
    colors_[3] = Gfx::Color(0.4f, 0.0f, 0.0f);
    colors_[4] = Gfx::Color(0.0f, 1.0f, 0.0f);
    colors_[5] = Gfx::Color(0.0f, 0.7f, 0.0f);
    colors_[6] = Gfx::Color(0.0f, 0.5f, 0.0f);
    colors_[7] = Gfx::Color(0.0f, 0.2f, 0.0f);
    colors_[8] = Gfx::Color(0.0f, 0.0f, 1.0f);
    colors_[9] = Gfx::Color(0.0f, 0.0f, 0.7f);
    colors_[10] = Gfx::Color(0.0f, 0.0f, 0.5f);
    colors_[11] = Gfx::Color(0.0f, 0.0f, 0.2f);
    colors_[12] = Gfx::Color(1.0f, 1.0f, 0.0f);
    colors_[13] = Gfx::Color(0.7f, 0.7f, 0.0f);
    colors_[14] = Gfx::Color(0.5f, 0.5f, 0.0f);
    colors_[15] = Gfx::Color(0.2f, 0.2f, 0.0f);
}

CellMapDocument::~CellMapDocument()
{
}

void CellMapDocument::InsertCell(const CellIndex& index, CellType type, float cell_size_, const std::string& ts)
{
    if(type == CT_SOLID_SPACE)
    {
        RemoveCell(index);
        return;
    }

    Cell c;
    c.type_     = type;
    c.center_x_ = (cell_size_ * float(index.x_)) + (cell_size_/2.0f);
    c.center_z_ = (cell_size_ * float(index.z_)) + (cell_size_/2.0f);
    c.min_x_    = cell_size_ * float(index.x_);
    c.max_x_    = c.min_x_ + cell_size_;
    c.min_z_    = cell_size_ * float(index.z_);
    c.max_z_    = c.min_z_ + cell_size_;

    TextureSetMap::iterator ts_itor = ts_map_.find(ts);
    if(ts_itor == ts_map_.end())
    {
        c.ts_name_   = default_texture_set_name_;
        c.ts_        = &default_texture_set_;
    }
    else
    {
        c.ts_name_   = ts;
        c.ts_        = &ts_itor->second;
    }

    CellMap_.Cells_[index] = c;

    // We need to setup this cell, but we also need to setup
    // the cells around this cell because this cell may affect
    // its neighbours.
    for(int z = index.z_-1; z <= index.z_+1; z++)
    {
        for(int x = index.x_-1; x <= index.x_+1; x++)
        {
            CellIndex ci(x, z);
            DetermineCellTextures(ci);
            BuildPolygons(ci);
        }
    }
}

void CellMapDocument::InsertCell(const CellIndex& index, const Cell& c)
{
    if(c.type_ == CT_SOLID_SPACE)
    {
        RemoveCell(index);
        return;
    }

    CellMap_.Cells_[index] = c;

    CellContainer::iterator i = CellMap_.Cells_.find(index);
    Cell& d = i->second;

    d.center_x_ = (cell_size_ * float(index.x_)) + (cell_size_/2.0f);
    d.center_z_ = (cell_size_ * float(index.z_)) + (cell_size_/2.0f);

    TextureSetMap::iterator ts_itor = ts_map_.find(d.ts_name_);
    if(ts_itor == ts_map_.end())
    {
        d.ts_name_   = default_texture_set_name_;
        d.ts_        = &default_texture_set_;
    }
    else
    {
        d.ts_ = &ts_itor->second;
    }

    // We need to setup this cell, but we also need to setup
    // the cells around this cell because this cell may affect
    // its neighbours.
    for(int z = index.z_-1; z <= index.z_+1; z++)
    {
        for(int x = index.x_-1; x <= index.x_+1; x++)
        {
            CellIndex ci(x, z);
            DetermineCellTextures(ci);
            BuildPolygons(ci);
        }
    }
}

void CellMapDocument::RemoveCell(const CellIndex& index)
{
    CellContainer::iterator itor = CellMap_.Cells_.find(index);
    if(itor != CellMap_.Cells_.end())
    {
        CellMap_.Cells_.erase(itor);

        // Re-setup the neighbouring CellMap_.Cells_
        for(int z = index.z_-1; z <= index.z_+1; z++)
        {
            for(int x = index.x_-1; x <= index.x_+1; x++)
            {
                CellIndex ci(x, z);
                DetermineCellTextures(ci);
                BuildPolygons(ci);
            }
        }
    }
}

void CellMapDocument::ClearAllCells()
{
    CellMap_.Points_.clear();
    CellMap_.Cells_.clear();
}

void CellMapDocument::Clear()
{
    cell_size_ = 128.0f;
    ambient_ = 25;

    CellMap_.Points_.clear();
    CellMap_.Cells_.clear();;
    name_.clear();
    ts_map_.clear();
    default_texture_set_name_.clear();
    sky_texture_filename_.clear();
    light_list_.clear();

    BlankLightMap_.reset();
    sky_texture_.reset();
    default_texture_set_.tex_wall_.reset();
    default_texture_set_.tex_ceiling_.reset();
    default_texture_set_.tex_floor_.reset();
    default_texture_set_.tex_liquid_.reset();
    default_texture_set_.tex_door_.reset();
    default_texture_set_.tex_trans_wall_.reset();
    default_texture_set_.tex_trans_door_.reset();
}

CellType CellMapDocument::GetCellType(const CellIndex& index) const
{
    CellContainer::const_iterator itor = CellMap_.Cells_.find(index);
    if(itor != CellMap_.Cells_.end())
    {
        return itor->second.type_;
    }
    return CT_SOLID_SPACE;
}

void CellMapDocument::SetTextureSet(const CellIndex& index, const std::string& ts)
{
    CellContainer::iterator i = CellMap_.Cells_.find(index);
    if(i != CellMap_.Cells_.end())
    {
        Cell& c = i->second;

        TextureSetMap::iterator ts_itor = ts_map_.find(ts);
        if(ts_itor == ts_map_.end())
        {
            c.ts_name_   = default_texture_set_name_;
            c.ts_        = &default_texture_set_;
        }
        else
        {
            c.ts_name_   = ts;
            c.ts_        = &ts_itor->second;
        }

        // Re-setup the neighbouring CellMap_.Cells_
        for(int z = index.z_-1; z <= index.z_+1; z++)
        {
            for(int x = index.x_-1; x <= index.x_+1; x++)
            {
                CellIndex ci(x, z);
                DetermineCellTextures(ci);
                BuildPolygons(ci);
            }
        }
    }
}

void CellMapDocument::SetSkyFlag(const CellIndex& index, bool sky)
{
    CellContainer::iterator i = CellMap_.Cells_.find(index);
    if(i != CellMap_.Cells_.end())
    {
        // Only some cell types are allowed to have sky at their top.
        switch(i->second.type_)
        {
        case CT_NORMAL:
        case CT_SECRET:
        case CT_PIT:
        case CT_LIQUID:
            {
                i->second.sky_ = sky;

                // Re-setup the neighbouring CellMap_.Cells_
                for(int z = index.z_-1; z <= index.z_+1; z++)
                {
                    for(int x = index.x_-1; x <= index.x_+1; x++)
                    {
                        CellIndex ci(x, z);
                        DetermineCellTextures(ci);
                        BuildPolygons(ci);
                    }
                }
                break;
            }
        // else bugger off
        }
    }
}

bool CellMapDocument::GetSkyFlag(const CellIndex& index) const
{
    CellContainer::const_iterator i = CellMap_.Cells_.find(index);
    return (i == CellMap_.Cells_.end() ? false : i->second.sky_);
}

std::string CellMapDocument::GetTextureSet(const CellIndex& index) const
{
    CellContainer::const_iterator itor = CellMap_.Cells_.find(index);
    return (itor == CellMap_.Cells_.end() ? std::string() : itor->second.ts_name_);
}

void CellMapDocument::InitialiseDefaultTextureSet(Util::ResourceContext* res_cxt)
{
    default_texture_set_.tex_wall_          = res_cxt->GetDefaultTexture();
    default_texture_set_.tex_ceiling_       = res_cxt->GetDefaultTexture();
    default_texture_set_.tex_floor_         = res_cxt->GetDefaultTexture();
    default_texture_set_.tex_liquid_        = res_cxt->GetDefaultTexture();
    default_texture_set_.tex_door_          = res_cxt->GetDefaultTexture();
    default_texture_set_.tex_trans_wall_    = res_cxt->GetDefaultTexture();
    default_texture_set_.tex_trans_door_    = res_cxt->GetDefaultTexture();
    default_texture_set_.liquid_color_      = Gfx::Color(0.0f, 0.0f, 0.75f);

    for(int j = 0; j < PIT_DEPTH+1; j++)
    {
        default_texture_set_.tc_y_[j] = ((float(j)*cell_size_)+cell_size_) / float(default_texture_set_.tex_wall_->Height());
    }
    default_texture_set_.tc_df_y_     = DOOR_FRAME_HEIGHT / float(default_texture_set_.tex_wall_->Height());
    default_texture_set_.tc_top_y_    = SKY_SCHOCHURE_HEIGHT / float(default_texture_set_.tex_wall_->Height());

    BlankLightMap_ = CreateBlankLightMap();
}

Cell& CellMapDocument::GetCell(const CellIndex& index) const
{
    CellContainer::const_iterator itor = CellMap_.Cells_.find(index);
    if(itor == CellMap_.Cells_.end())
    {
        std::ostringstream o;
        o << "Invalid cell index supplied [" << index.x_ << ", " << index.z_ << "]";
        throw std::runtime_error(o.str());
    }
    return (Cell&)itor->second;
}

void CellMapDocument::BuildTextureSetMap(Util::ResourceContext* res_cxt, TextureSetDocumentMap* ts_docs, const std::string& content_dir)
{
    // Build the ts_map_ container
    TextureSetDocumentMap::iterator i;
    for(i = ts_docs->begin(); i != ts_docs->end(); ++i)
    {
        TextureSet ts;
        if(i->second.TexWall().empty())
        {
            ts.tex_wall_ = res_cxt->GetDefaultTexture();
        }
        else
        {
            ts.tex_wall_ = res_cxt->FindTexture(content_dir + "\\Textures\\" + i->second.TexWall());
        }
        if(i->second.TexCeiling().empty())
        {
            ts.tex_ceiling_ = res_cxt->GetDefaultTexture();
        }
        else
        {
            ts.tex_ceiling_ = res_cxt->FindTexture(content_dir + "\\Textures\\" + i->second.TexCeiling());
        }
        if(i->second.TexFloor().empty())
        {
            ts.tex_floor_ = res_cxt->GetDefaultTexture();
        }
        else
        {
            ts.tex_floor_ = res_cxt->FindTexture(content_dir + "\\Textures\\" + i->second.TexFloor());
        }
        if(i->second.TexLiquid().empty())
        {
            ts.tex_liquid_ = res_cxt->GetDefaultTexture();
        }
        else
        {
            ts.tex_liquid_ = res_cxt->FindTexture(content_dir + "\\Textures\\" + i->second.TexLiquid());
        }
        if(i->second.TexDoor().empty())
        {
            ts.tex_door_ = res_cxt->GetDefaultTexture();
        }
        else
        {
            ts.tex_door_ = res_cxt->FindTexture(content_dir + "\\Textures\\" + i->second.TexDoor());
        }
        if(i->second.TexTransWall().empty())
        {
            ts.tex_trans_wall_ = res_cxt->GetDefaultTexture();
        }
        else
        {
            ts.tex_trans_wall_ = res_cxt->FindTexture(content_dir + "\\Textures\\" + i->second.TexTransWall());
        }
        if(i->second.TexTransDoor().empty())
        {
            ts.tex_trans_door_ = res_cxt->GetDefaultTexture();
        }
        else
        {
            ts.tex_trans_door_ = res_cxt->FindTexture(content_dir + "\\Textures\\" + i->second.TexTransDoor());
        }

        for(int j = 0; j < PIT_DEPTH+1; j++)
        {
            ts.tc_y_[j] = ((float(j)*cell_size_)+cell_size_) / float(ts.tex_wall_->Height());
        }
        ts.tc_df_y_     = DOOR_FRAME_HEIGHT / float(ts.tex_wall_->Height());
        ts.tc_top_y_    = SKY_SCHOCHURE_HEIGHT / float(ts.tex_wall_->Height());

        ts_map_[i->first] = ts;
    }
}

void CellMapDocument::BindCellsToTextures(boost::function<void (int)> total_function, boost::function<void (int)> step_function)
{
    total_function(int(CellMap_.Cells_.size()));

    unsigned long time_prev = timeGetTime();

    // Bind each cell to its Texture Set in the ts_map_ container
    CellContainer::iterator c;
    int i = 0;
    for(c = CellMap_.Cells_.begin(); c != CellMap_.Cells_.end(); ++c, ++i)
    {
        TextureSetMap::iterator ts = ts_map_.find(c->second.ts_name_);
        if(ts == ts_map_.end())
        {
            c->second.ts_name_   = default_texture_set_name_;
            c->second.ts_        = &default_texture_set_;
        }
        else
        {
            c->second.ts_ = &ts->second;
        }

        unsigned long now = timeGetTime();
        if(now - time_prev >= 100)
        {
            step_function(i);
            time_prev = now;
        }
    }

    step_function(int(CellMap_.Cells_.size()));
}

void CellMapDocument::Think(float time_delta)
{
    // This is temporary :-)
    //CellContainer::iterator c;
    //for(c = CellMap_.Cells_.begin(); c != CellMap_.Cells_.end(); ++c)
    //{
    //    switch(c->second.type_)
    //    {
    //    case CT_DOOR:
    //    case CT_SECRET_DOOR:
    //    case CT_TRANSLUCENT_DOOR:
    //        switch(c->second.door_state_)
    //        {
    //        case Cell::DS_CLOSED:
    //            c->second.door_state_ = Cell::DS_OPENING;
    //            break;
    //        case Cell::DS_OPENED:
    //            c->second.door_state_ = Cell::DS_CLOSING;
    //            break;
    //        case Cell::DS_OPENING:
    //            if(c->second.door_offset_.y_ < 110.0f)
    //            {
    //                c->second.door_offset_.y_ += 50.0f*time_delta;
    //            }
    //            else
    //            {
    //                c->second.door_offset_.y_   = 110.0f;
    //                c->second.door_state_       = Cell::DS_OPENED;
    //            }
    //            break;
    //        case Cell::DS_CLOSING:
    //            if(c->second.door_offset_.y_ > 0.0f)
    //            {
    //                c->second.door_offset_.y_ -= 50.0f*time_delta;
    //            }
    //            else
    //            {
    //                c->second.door_offset_.y_   = 0.0f;
    //                c->second.door_state_       = Cell::DS_CLOSED;
    //            }
    //            break;
    //        }
    //        break;
    //    }
    //}
}

void CellMapDocument::DrawCellsOpaque_Textured()
{
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);

    CellContainer::const_iterator i;
    for(i = CellMap_.Cells_.begin(); i != CellMap_.Cells_.end(); ++i)
    {
        const Cell& c = i->second;

        Gfx::PolygonList::const_iterator j;
        for(j = c.polygons_opaque_quads_.begin(); j != c.polygons_opaque_quads_.end(); ++j)
        {
            const Gfx::Polygon& p = *j;
            glActiveTextureARB(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, p.TexColor_->Id());
            glActiveTextureARB(GL_TEXTURE1_ARB); glBindTexture(GL_TEXTURE_2D, p.TexLightMap_->Id());
            glBegin(GL_TRIANGLE_FAN);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[0].u_, p.TexCoords_[0].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[0]].x_, CellMap_.Points_[p.Indices_[0]].y_, CellMap_.Points_[p.Indices_[0]].z_);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[1].u_, p.TexCoords_[1].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[1]].x_, CellMap_.Points_[p.Indices_[1]].y_, CellMap_.Points_[p.Indices_[1]].z_);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[2].u_, p.TexCoords_[2].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[2]].x_, CellMap_.Points_[p.Indices_[2]].y_, CellMap_.Points_[p.Indices_[2]].z_);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[3].u_, p.TexCoords_[3].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[3]].x_, CellMap_.Points_[p.Indices_[3]].y_, CellMap_.Points_[p.Indices_[3]].z_);
            glEnd();
        }

        for(j = c.polygons_opaque_tris_.begin(); j != c.polygons_opaque_tris_.end(); ++j)
        {
            const Gfx::Polygon& p = *j;
            glActiveTextureARB(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, p.TexColor_->Id());
            glActiveTextureARB(GL_TEXTURE1_ARB); glBindTexture(GL_TEXTURE_2D, p.TexLightMap_->Id());
            glBegin(GL_TRIANGLES);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[0].u_, p.TexCoords_[0].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[0]].x_, CellMap_.Points_[p.Indices_[0]].y_, CellMap_.Points_[p.Indices_[0]].z_);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[1].u_, p.TexCoords_[1].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[1]].x_, CellMap_.Points_[p.Indices_[1]].y_, CellMap_.Points_[p.Indices_[1]].z_);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[2].u_, p.TexCoords_[2].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[2]].x_, CellMap_.Points_[p.Indices_[2]].y_, CellMap_.Points_[p.Indices_[2]].z_);
            glEnd();
        }

        if(!c.polygons_door_.empty())
        {
            glPushMatrix();
            glTranslatef(c.door_offset_.x_, c.door_offset_.y_, c.door_offset_.z_);

            for(j = c.polygons_door_.begin(); j != c.polygons_door_.end(); ++j)
            {
                const Gfx::Polygon& p = *j;
                glActiveTextureARB(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, p.TexColor_->Id());
                glActiveTextureARB(GL_TEXTURE1_ARB); glBindTexture(GL_TEXTURE_2D, p.TexLightMap_->Id());
                glBegin(GL_TRIANGLE_FAN);
                    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[0].u_, p.TexCoords_[0].v_);
                    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
                    glVertex3f(CellMap_.Points_[p.Indices_[0]].x_, CellMap_.Points_[p.Indices_[0]].y_, CellMap_.Points_[p.Indices_[0]].z_);
                    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[1].u_, p.TexCoords_[1].v_);
                    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
                    glVertex3f(CellMap_.Points_[p.Indices_[1]].x_, CellMap_.Points_[p.Indices_[1]].y_, CellMap_.Points_[p.Indices_[1]].z_);
                    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[2].u_, p.TexCoords_[2].v_);
                    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
                    glVertex3f(CellMap_.Points_[p.Indices_[2]].x_, CellMap_.Points_[p.Indices_[2]].y_, CellMap_.Points_[p.Indices_[2]].z_);
                    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[3].u_, p.TexCoords_[3].v_);
                    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
                    glVertex3f(CellMap_.Points_[p.Indices_[3]].x_, CellMap_.Points_[p.Indices_[3]].y_, CellMap_.Points_[p.Indices_[3]].z_);
                glEnd();
            }

            glPopMatrix();
        }
    }
}

void CellMapDocument::DrawCellsOpaque_Colored()
{
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    current_color_ = 0;

    CellContainer::const_iterator i;
    for(i = CellMap_.Cells_.begin(); i != CellMap_.Cells_.end(); ++i)
    {
        const Cell& c = i->second;

        glColor3f(colors_[current_color_].r_, colors_[current_color_].g_, colors_[current_color_].b_);
        if(++current_color_ >= 16) current_color_ = 0;

        Gfx::PolygonList::const_iterator j;
        for(j = c.polygons_opaque_quads_.begin(); j != c.polygons_opaque_quads_.end(); ++j)
        {
            const Gfx::Polygon& p = *j;
            glBegin(GL_TRIANGLE_FAN);
                glVertex3f(CellMap_.Points_[p.Indices_[0]].x_, CellMap_.Points_[p.Indices_[0]].y_, CellMap_.Points_[p.Indices_[0]].z_);
                glVertex3f(CellMap_.Points_[p.Indices_[1]].x_, CellMap_.Points_[p.Indices_[1]].y_, CellMap_.Points_[p.Indices_[1]].z_);
                glVertex3f(CellMap_.Points_[p.Indices_[2]].x_, CellMap_.Points_[p.Indices_[2]].y_, CellMap_.Points_[p.Indices_[2]].z_);
                glVertex3f(CellMap_.Points_[p.Indices_[3]].x_, CellMap_.Points_[p.Indices_[3]].y_, CellMap_.Points_[p.Indices_[3]].z_);
            glEnd();
        }

        glColor3f(colors_[current_color_].r_, colors_[current_color_].g_, colors_[current_color_].b_);
        if(++current_color_ >= 16) current_color_ = 0;

        for(j = c.polygons_opaque_tris_.begin(); j != c.polygons_opaque_tris_.end(); ++j)
        {
            const Gfx::Polygon& p = *j;
            glBegin(GL_TRIANGLES);
                glVertex3f(CellMap_.Points_[p.Indices_[0]].x_, CellMap_.Points_[p.Indices_[0]].y_, CellMap_.Points_[p.Indices_[0]].z_);
                glVertex3f(CellMap_.Points_[p.Indices_[1]].x_, CellMap_.Points_[p.Indices_[1]].y_, CellMap_.Points_[p.Indices_[1]].z_);
                glVertex3f(CellMap_.Points_[p.Indices_[2]].x_, CellMap_.Points_[p.Indices_[2]].y_, CellMap_.Points_[p.Indices_[2]].z_);
            glEnd();
        }

        if(!c.polygons_door_.empty())
        {
            glColor3f(colors_[current_color_].r_, colors_[current_color_].g_, colors_[current_color_].b_);
            if(++current_color_ >= 16) current_color_ = 0;

            glPushMatrix();
            glTranslatef(c.door_offset_.x_, c.door_offset_.y_, c.door_offset_.z_);

            for(j = c.polygons_door_.begin(); j != c.polygons_door_.end(); ++j)
            {
                const Gfx::Polygon& p = *j;
                glBegin(GL_TRIANGLE_FAN);
                    glVertex3f(CellMap_.Points_[p.Indices_[0]].x_, CellMap_.Points_[p.Indices_[0]].y_, CellMap_.Points_[p.Indices_[0]].z_);
                    glVertex3f(CellMap_.Points_[p.Indices_[1]].x_, CellMap_.Points_[p.Indices_[1]].y_, CellMap_.Points_[p.Indices_[1]].z_);
                    glVertex3f(CellMap_.Points_[p.Indices_[2]].x_, CellMap_.Points_[p.Indices_[2]].y_, CellMap_.Points_[p.Indices_[2]].z_);
                    glVertex3f(CellMap_.Points_[p.Indices_[3]].x_, CellMap_.Points_[p.Indices_[3]].y_, CellMap_.Points_[p.Indices_[3]].z_);
                glEnd();
            }

            glPopMatrix();
        }
    }
}

void CellMapDocument::DrawCellsTranslucent_Textured()
{
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);

    CellContainer::const_iterator i;
    for(i = CellMap_.Cells_.begin(); i != CellMap_.Cells_.end(); ++i)
    {
        const Cell& c = i->second;

        Gfx::PolygonList::const_iterator j;
        for(j = c.polygons_translucent_.begin(); j != c.polygons_translucent_.end(); ++j)
        {
            const Gfx::Polygon& p = *j;
            glActiveTextureARB(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, p.TexColor_->Id());
            glActiveTextureARB(GL_TEXTURE1_ARB); glBindTexture(GL_TEXTURE_2D, p.TexLightMap_->Id());
            glBegin(GL_TRIANGLE_FAN);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[0].u_, p.TexCoords_[0].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[0]].x_, CellMap_.Points_[p.Indices_[0]].y_, CellMap_.Points_[p.Indices_[0]].z_);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[1].u_, p.TexCoords_[1].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[1]].x_, CellMap_.Points_[p.Indices_[1]].y_, CellMap_.Points_[p.Indices_[1]].z_);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[2].u_, p.TexCoords_[2].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[2]].x_, CellMap_.Points_[p.Indices_[2]].y_, CellMap_.Points_[p.Indices_[2]].z_);
                glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[3].u_, p.TexCoords_[3].v_);
                glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
                glVertex3f(CellMap_.Points_[p.Indices_[3]].x_, CellMap_.Points_[p.Indices_[3]].y_, CellMap_.Points_[p.Indices_[3]].z_);
            glEnd();
        }

        if(!c.polygons_door_translucent_.empty())
        {
            glPushMatrix();
            glTranslatef(c.door_offset_.x_, c.door_offset_.y_, c.door_offset_.z_);

            for(j = c.polygons_door_translucent_.begin(); j != c.polygons_door_translucent_.end(); ++j)
            {
                const Gfx::Polygon& p = *j;
                glActiveTextureARB(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, p.TexColor_->Id());
                glActiveTextureARB(GL_TEXTURE1_ARB); glBindTexture(GL_TEXTURE_2D, p.TexLightMap_->Id());
                glBegin(GL_TRIANGLE_FAN);
                    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[0].u_, p.TexCoords_[0].v_);
                    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
                    glVertex3f(CellMap_.Points_[p.Indices_[0]].x_, CellMap_.Points_[p.Indices_[0]].y_, CellMap_.Points_[p.Indices_[0]].z_);
                    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[1].u_, p.TexCoords_[1].v_);
                    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
                    glVertex3f(CellMap_.Points_[p.Indices_[1]].x_, CellMap_.Points_[p.Indices_[1]].y_, CellMap_.Points_[p.Indices_[1]].z_);
                    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[2].u_, p.TexCoords_[2].v_);
                    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
                    glVertex3f(CellMap_.Points_[p.Indices_[2]].x_, CellMap_.Points_[p.Indices_[2]].y_, CellMap_.Points_[p.Indices_[2]].z_);
                    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, p.TexCoords_[3].u_, p.TexCoords_[3].v_);
                    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
                    glVertex3f(CellMap_.Points_[p.Indices_[3]].x_, CellMap_.Points_[p.Indices_[3]].y_, CellMap_.Points_[p.Indices_[3]].z_);
                glEnd();
            }

            glPopMatrix();
        }
    }

    glDisable(GL_BLEND);
}

void CellMapDocument::DrawSky()
{
    if(sky_texture_)
    {
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sky_texture_->Id());

        glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f); glVertex3f(min_sky_x_, 100.0f, max_sky_z_);
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f); glVertex3f(min_sky_x_, 100.0f, min_sky_z_);
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 1.0f); glVertex3f(max_sky_x_, 100.0f, min_sky_z_);
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 0.0f); glVertex3f(max_sky_x_, 100.0f, max_sky_z_);
        glEnd();
    }
}

int CellMapDocument::GetHighestLightNumber() const
{
    int number = -1;
    LightList::const_iterator i;
    for(i = light_list_.begin(); i != light_list_.end(); ++i)
    {
        if(boost::algorithm::iequals(i->Name().substr(0, 5), "Light"))
        {
            try
            {
                std::string str(i->Name().substr(5, std::string::npos));
                int temp = boost::lexical_cast<int>(str);
                if(temp > number)
                {
                    number = temp;
                }
            }
            catch(boost::bad_lexical_cast& )
            {
            }
        }
    }
    return number;
}

void CellMapDocument::DetermineCellTextures(const CellIndex& index)
{
    CellContainer::iterator i = CellMap_.Cells_.find(index);
    if(i == CellMap_.Cells_.end())
    {
        return;
    }
    Cell& c = i->second;
    if((c.type_ != CT_DOOR) && (c.type_ != CT_TRANSLUCENT_DOOR) &&
       (c.type_ != CT_TRANSLUCENT_SMALL_WALL) && (c.type_ != CT_TRANSLUCENT_LARGE_WALL))
    {
        return;
    }

    CellIndex ci(index.x_, index.z_-1);
    bool cell_above = (!IsSolidSpace(ci) && GetCell(ci).type_ != GetCell(index).type_);

    ci = CellIndex(index.x_, index.z_+1);
    bool cell_below = (!IsSolidSpace(ci) && GetCell(ci).type_ != GetCell(index).type_);

    ci = CellIndex(index.x_-1, index.z_);
    bool cell_left  = (!IsSolidSpace(ci) && GetCell(ci).type_ != GetCell(index).type_);

    ci = CellIndex(index.x_+1, index.z_);
    bool cell_right = (!IsSolidSpace(ci) && GetCell(ci).type_ != GetCell(index).type_);

    if(cell_above)      c.tex_left_top_ = GetCell(CellIndex(index.x_, index.z_-1)).ts_;
    else if(cell_below) c.tex_left_top_ = GetCell(CellIndex(index.x_, index.z_+1)).ts_;
    else if(cell_right) c.tex_left_top_ = GetCell(CellIndex(index.x_+1, index.z_)).ts_;
    else if(!IsSolidSpace(CellIndex(index.x_-1, index.z_-1))) c.tex_left_top_ = GetCell(CellIndex(index.x_-1, index.z_-1)).ts_;
    else                c.tex_left_top_ = c.ts_;

    if(cell_below)      c.tex_left_bot_ = GetCell(CellIndex(index.x_, index.z_+1)).ts_;
    else if(cell_above) c.tex_left_bot_ = GetCell(CellIndex(index.x_, index.z_-1)).ts_;
    else if(cell_right) c.tex_left_bot_ = GetCell(CellIndex(index.x_+1, index.z_)).ts_;
    else if(!IsSolidSpace(CellIndex(index.x_-1, index.z_+1))) c.tex_left_bot_ = GetCell(CellIndex(index.x_-1, index.z_+1)).ts_;
    else                c.tex_left_bot_ = c.ts_;

    if(cell_left)       c.tex_top_left_ = GetCell(CellIndex(index.x_-1, index.z_)).ts_;
    else if(cell_right) c.tex_top_left_ = GetCell(CellIndex(index.x_+1, index.z_)).ts_;
    else if(cell_below) c.tex_top_left_ = GetCell(CellIndex(index.x_, index.z_+1)).ts_;
    else if(!IsSolidSpace(CellIndex(index.x_-1, index.z_-1))) c.tex_top_left_ = GetCell(CellIndex(index.x_-1, index.z_-1)).ts_;
    else                c.tex_top_left_ = c.ts_;

    if(cell_right)      c.tex_top_right_ = GetCell(CellIndex(index.x_+1, index.z_)).ts_;
    else if(cell_left)  c.tex_top_right_ = GetCell(CellIndex(index.x_-1, index.z_)).ts_;
    else if(cell_below) c.tex_top_right_ = GetCell(CellIndex(index.x_, index.z_+1)).ts_;
    else if(!IsSolidSpace(CellIndex(index.x_+1, index.z_-1))) c.tex_top_right_ = GetCell(CellIndex(index.x_+1, index.z_-1)).ts_;
    else                c.tex_top_right_ = c.ts_;

    if(cell_above)      c.tex_right_top_ = GetCell(CellIndex(index.x_, index.z_-1)).ts_;
    else if(cell_below) c.tex_right_top_ = GetCell(CellIndex(index.x_, index.z_+1)).ts_;
    else if(cell_left)  c.tex_right_top_ = GetCell(CellIndex(index.x_-1, index.z_)).ts_;
    else if(!IsSolidSpace(CellIndex(index.x_+1, index.z_-1))) c.tex_right_top_ = GetCell(CellIndex(index.x_+1, index.z_-1)).ts_;
    else                c.tex_right_top_ = c.ts_;

    if(cell_below)      c.tex_right_bot_ = GetCell(CellIndex(index.x_, index.z_+1)).ts_;
    else if(cell_above) c.tex_right_bot_ = GetCell(CellIndex(index.x_, index.z_-1)).ts_;
    else if(cell_left)  c.tex_right_bot_ = GetCell(CellIndex(index.x_-1, index.z_)).ts_;
    else if(!IsSolidSpace(CellIndex(index.x_+1, index.z_+1))) c.tex_right_bot_ = GetCell(CellIndex(index.x_+1, index.z_+1)).ts_;
    else                c.tex_right_bot_ = c.ts_;

    if(cell_left)       c.tex_bot_left_ = GetCell(CellIndex(index.x_-1, index.z_)).ts_;
    else if(cell_right) c.tex_bot_left_ = GetCell(CellIndex(index.x_+1, index.z_)).ts_;
    else if(cell_above) c.tex_bot_left_ = GetCell(CellIndex(index.x_, index.z_-1)).ts_;
    else if(!IsSolidSpace(CellIndex(index.x_-1, index.z_+1))) c.tex_bot_left_ = GetCell(CellIndex(index.x_-1, index.z_+1)).ts_;
    else                c.tex_bot_left_ = c.ts_;

    if(cell_right)      c.tex_bot_right_ = GetCell(CellIndex(index.x_+1, index.z_)).ts_;
    else if(cell_left)  c.tex_bot_right_ = GetCell(CellIndex(index.x_-1, index.z_)).ts_;
    else if(cell_above) c.tex_bot_right_ = GetCell(CellIndex(index.x_, index.z_-1)).ts_;
    else if(!IsSolidSpace(CellIndex(index.x_+1, index.z_+1))) c.tex_bot_right_ = GetCell(CellIndex(index.x_+1, index.z_+1)).ts_;
    else                c.tex_bot_right_ = c.ts_;
}

bool CellMapDocument::IsSolidSpace(const CellIndex& i) const
{
    CellContainer::const_iterator itor = CellMap_.Cells_.find(i);
    if(itor == CellMap_.Cells_.end())
    {
        return true;
    }
    return itor->second.type_ == CT_SOLID_SPACE;
}

bool CellMapDocument::IsSameType(const CellIndex& i, CellType type) const
{
    CellContainer::const_iterator itor = CellMap_.Cells_.find(i);
    if(itor == CellMap_.Cells_.end())
    {
        return true;
    }
    return itor->second.type_ == type;
}

bool CellMapDocument::IsSky(const CellIndex& i) const
{
    CellContainer::const_iterator itor = CellMap_.Cells_.find(i);
    if(itor == CellMap_.Cells_.end())
    {
        return false;
    }
    return itor->second.sky_;
}

void CellMapDocument::BuildSkyPolygon(Util::ResourceContext* res_cxt, const std::string& content_dir)
{
    if(!sky_texture_filename_.empty())
    {
        sky_texture_ = res_cxt->LoadTexture(content_dir + "\\Textures\\" + sky_texture_filename_);
        CalcSkyDimensions();
    }
}

void CellMapDocument::BuildCombinedCellBspTrees(boost::function<void (int)> total_function, boost::function<void (int)> step_function)
{
    total_function(int(CellMap_.Cells_.size()));

    unsigned long time_prev = timeGetTime();

    CellContainer::iterator i;
    int k = 0;
    for(i = CellMap_.Cells_.begin(); i != CellMap_.Cells_.end(); ++i, ++k)
    {
        // Create the current cell's bsp tree
        i->second.bsp_tree_.Clear();
        AddCellsPlanesToBsp(i, i->second.bsp_tree_);

        // Add all the surrounding cell's bsp trees to the current cell's combined tree.
        CellContainer::iterator j = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_-1));
        if(j != CellMap_.Cells_.end()) AddCellsPlanesToBsp(j, i->second.bsp_tree_);

        j = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_-1));
        if(j != CellMap_.Cells_.end()) AddCellsPlanesToBsp(j, i->second.bsp_tree_);

        j = CellMap_.Cells_.find(CellIndex(i->first.x_+1, i->first.z_-1));
        if(j != CellMap_.Cells_.end()) AddCellsPlanesToBsp(j, i->second.bsp_tree_);

        j = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_));
        if(j != CellMap_.Cells_.end()) AddCellsPlanesToBsp(j, i->second.bsp_tree_);

        j = CellMap_.Cells_.find(CellIndex(i->first.x_+1, i->first.z_));
        if(j != CellMap_.Cells_.end()) AddCellsPlanesToBsp(j, i->second.bsp_tree_);

        j = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_+1));
        if(j != CellMap_.Cells_.end()) AddCellsPlanesToBsp(j, i->second.bsp_tree_);

        j = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_+1));
        if(j != CellMap_.Cells_.end()) AddCellsPlanesToBsp(j, i->second.bsp_tree_);

        j = CellMap_.Cells_.find(CellIndex(i->first.x_+1, i->first.z_+1));
        if(j != CellMap_.Cells_.end()) AddCellsPlanesToBsp(j, i->second.bsp_tree_);

        unsigned long now = timeGetTime();
        if(now - time_prev >= 100)
        {
            step_function(k);
            time_prev = now;
        }
    }

    step_function(int(CellMap_.Cells_.size()));
}

void CellMapDocument::AddCellsPlanesToBsp(const CellContainer::iterator& itor, Bsp::Tree& tree)
{
    Cell& c = itor->second;

    Math::Plane p;
    Math::Vector v0, v1;

    switch(c.type_)
    {
    case CT_SOLID_SPACE:
        return;
    case CT_DOOR:
    case CT_SECRET_DOOR:
    case CT_TRANSLUCENT_DOOR:
        // Need to add a dynamic tree for the door itself...

        // Fall through
    case CT_NORMAL:
    case CT_SECRET:
        // Floor
        v0 = Math::Vector(c.center_x_, -cell_size_, c.center_z_);
        v1 = Math::Vector(c.center_x_, -cell_size_, c.center_z_-10.0f);
        p.on_plane_ = v0;
        p.normal_ = Math::Vector(0.0f, 1.0f, 0.0f);
        tree.Insert(p, v1);
        break;
    case CT_TRANSLUCENT_SMALL_WALL:
    case CT_TRANSLUCENT_LARGE_WALL:
        // Need to add other planes for the internal structure of the walls...

        // Floor
        v0 = Math::Vector(c.center_x_, -cell_size_, c.center_z_);
        v1 = Math::Vector(c.center_x_, -cell_size_, c.center_z_-10.0f);
        p.on_plane_ = v0;
        p.normal_ = Math::Vector(0.0f, 1.0f, 0.0f);
        tree.Insert(p, v1);
        break;
    case CT_PIT:
        // Floor
        v0 = Math::Vector(c.center_x_, (float(PIT_DEPTH) * -cell_size_) - cell_size_, c.center_z_);
        v1 = Math::Vector(c.center_x_, (float(PIT_DEPTH) * -cell_size_) - cell_size_, c.center_z_-10.0f);
        p.on_plane_ = v0;
        p.normal_ = Math::Vector(0.0f, 1.0f, 0.0f);
        tree.Insert(p, v1);
        break;
    case CT_LIQUID:
        // Floor
        v0 = Math::Vector(c.center_x_, -cell_size_*2.0f, c.center_z_);
        v1 = Math::Vector(c.center_x_, -cell_size_*2.0f, c.center_z_-10.0f);
        p.on_plane_ = v0;
        p.normal_ = Math::Vector(0.0f, 1.0f, 0.0f);
        tree.Insert(p, v1);
        break;
    }

    // Ceiling
    v0 = Math::Vector(c.center_x_, 0.0f, c.center_z_);
    v1 = Math::Vector(c.center_x_, 0.0f, c.center_z_-10.0f);
    p.on_plane_ = v0;
    p.normal_ = Math::Vector(0.0f, -1.0f, 0.0f);
    tree.Insert(p, v1);

    // Left
    if(IsSolidSpace(CellIndex(itor->first.x_-1, itor->first.z_)))
    {
        v0 = Math::Vector(c.min_x_, -cell_size_/2.0f, c.center_z_);
        v1 = Math::Vector(c.min_x_, -cell_size_/2.0f, c.center_z_-10.0f);
        p.on_plane_ = v0;
        p.normal_ = Math::Vector(1.0f, 0.0f, 0.0f);
        tree.Insert(p, v1);
    }

    // Right
    if(IsSolidSpace(CellIndex(itor->first.x_+1, itor->first.z_)))
    {
        v0 = Math::Vector(c.max_x_, -cell_size_/2.0f, c.center_z_);
        v1 = Math::Vector(c.max_x_, -cell_size_/2.0f, c.center_z_-10.0f);
        p.on_plane_ = v0;
        p.normal_ = Math::Vector(-1.0f, 0.0f, 0.0f);
        tree.Insert(p, v1);
    }

    // Front
    if(IsSolidSpace(CellIndex(itor->first.x_, itor->first.z_+1)))
    {
        v0 = Math::Vector(c.center_x_, -cell_size_/2.0f, c.max_z_);
        v1 = Math::Vector(c.center_x_-10.0f, -cell_size_/2.0f, c.max_z_);
        p.on_plane_ = v0;
        p.normal_ = Math::Vector(0.0f, 0.0f, -1.0f);
        tree.Insert(p, v1);
    }

    // Back
    if(IsSolidSpace(CellIndex(itor->first.x_, itor->first.z_-1)))
    {
        v0 = Math::Vector(c.center_x_, -cell_size_/2.0f, c.min_z_);
        v1 = Math::Vector(c.center_x_-10.0f, -cell_size_/2.0f, c.min_z_);
        p.on_plane_ = v0;
        p.normal_ = Math::Vector(0.0f, 0.0f, 1.0f);
        tree.Insert(p, v1);
    }

    // For the pit and liquid cell types there's further vertical planes to add to the tree.
    if(c.type_ == CT_PIT || c.type_ == CT_LIQUID)
    {
        // Left
        if(!IsSameType(CellIndex(itor->first.x_-1, itor->first.z_), c.type_))
        {
            v0 = Math::Vector(c.min_x_, -cell_size_ - (cell_size_/2.0f), c.center_z_);
            v1 = Math::Vector(c.min_x_, -cell_size_ - (cell_size_/2.0f), c.center_z_-10.0f);
            p.on_plane_ = v0;
            p.normal_ = Math::Vector(1.0f, 0.0f, 0.0f);
            tree.Insert(p, v1);
        }

        // Right
        if(!IsSameType(CellIndex(itor->first.x_+1, itor->first.z_), c.type_))
        {
            v0 = Math::Vector(c.max_x_, -cell_size_ - (cell_size_/2.0f), c.center_z_);
            v1 = Math::Vector(c.max_x_, -cell_size_ - (cell_size_/2.0f), c.center_z_-10.0f);
            p.on_plane_ = v0;
            p.normal_ = Math::Vector(-1.0f, 0.0f, 0.0f);
            tree.Insert(p, v1);
        }

        // Front
        if(!IsSameType(CellIndex(itor->first.x_, itor->first.z_+1), c.type_))
        {
            v0 = Math::Vector(c.center_x_, -cell_size_ - (cell_size_/2.0f), c.max_z_);
            v1 = Math::Vector(c.center_x_-10.0f, -cell_size_ - (cell_size_/2.0f), c.max_z_);
            p.on_plane_ = v0;
            p.normal_ = Math::Vector(0.0f, 0.0f, -1.0f);
            tree.Insert(p, v1);
        }

        // Back
        if(!IsSameType(CellIndex(itor->first.x_, itor->first.z_-1), c.type_))
        {
            v0 = Math::Vector(c.center_x_, -cell_size_ - (cell_size_/2.0f), c.min_z_);
            v1 = Math::Vector(c.center_x_-10.0f, -cell_size_ - (cell_size_/2.0f), c.min_z_);
            p.on_plane_ = v0;
            p.normal_ = Math::Vector(0.0f, 0.0f, 1.0f);
            tree.Insert(p, v1);
        }
    }
}

bool CellMapDocument::IsLineOfSight(const Math::LineSegment& ls, float plane_shift) const
{
    CellIndex ci(int(ls.begin_.x_/cell_size_), int(ls.begin_.z_/cell_size_));
    CellContainer::const_iterator i = CellMap_.Cells_.find(ci);
    if(i != CellMap_.Cells_.end())
    {
        return i->second.bsp_tree_.IsLineOfSight(ls, plane_shift);
    }
    return true;
}

Math::Vector CellMapDocument::TraceLineSegment(const Math::LineSegment& ls, float plane_shift) const
{
    CellIndex ci(int(ls.begin_.x_/cell_size_), int(ls.begin_.z_/cell_size_));
    CellContainer::const_iterator i = CellMap_.Cells_.find(ci);
    if(i != CellMap_.Cells_.end())
    {
        return i->second.bsp_tree_.TraceLineSegment_Slide(ls, plane_shift);
    }
    return ls.begin_;
}

Light* CellMapDocument::GetLight(const std::string& name) const
{
    LightList::const_iterator i = std::find(light_list_.begin(), light_list_.end(), name);
    return i == light_list_.end() ? NULL : (Light*)&(*i);
}

Gfx::TexturePtr CellMapDocument::CreateBlankLightMap() const
{
    const int Size = LIGHT_MAP_SIZE * LIGHT_MAP_SIZE * 3;

    boost::shared_array<unsigned char> Pixels;
    Pixels.reset(new unsigned char[Size]);
    memset(Pixels.get(), 255, sizeof(unsigned char)*Size);

    Gfx::TexturePtr Tex(new Gfx::Texture);
    Tex->CreateBilinear(LIGHT_MAP_SIZE, LIGHT_MAP_SIZE, 24, Pixels);
    return Tex;
}

void CellMapDocument::SetupPolygonsLightMap(Gfx::Polygon& P)
{
    // X axis of matrix points in world space direction of the U texture axis
    P.Matrix_.V_[0] = CellMap_.Points_[P.Indices_[3]] - CellMap_.Points_[P.Indices_[0]];
    P.ULength_ = P.Matrix_.V_[0].Magnitude();
    P.Matrix_.V_[0].Normalise();

    // Y axis of matrix points in world space direction of the V texture axis
    P.Matrix_.V_[1] = CellMap_.Points_[P.Indices_[1]] - CellMap_.Points_[P.Indices_[0]];
    P.VLength_ = P.Matrix_.V_[1].Magnitude();
    P.Matrix_.V_[1].Normalise();

    // Z axis of matrix is the surface's normal
    P.Matrix_.V_[2] = P.Matrix_.V_[0].CrossProduct(P.Matrix_.V_[1]);
}

void CellMapDocument::CalcSkyDimensions()
{
    int min_cell_x = 100000;
    int max_cell_x = -100000;
    int min_cell_z = 100000;
    int max_cell_z = -100000;

    CellContainer::iterator i;
    for(i = CellMap_.Cells_.begin(); i != CellMap_.Cells_.end(); ++i)
    {
        if(i->first.x_ < min_cell_x) min_cell_x = i->first.x_;
        if(i->first.z_ < min_cell_z) min_cell_z = i->first.z_;
        if(i->first.x_ > max_cell_x) max_cell_x = i->first.x_;
        if(i->first.z_ > max_cell_z) max_cell_z = i->first.z_;
    }

    min_cell_x -= 100;
    max_cell_x += 100;
    min_cell_z -= 100;
    max_cell_z += 100;

    min_sky_x_ = float(min_cell_x) * cell_size_;
    max_sky_x_ = float(max_cell_x) * cell_size_;
    min_sky_z_ = float(min_cell_z) * cell_size_;
    max_sky_z_ = float(max_cell_z) * cell_size_;
}


void CellMapDocument::BuildPolygons(const CellIndex& ci)
{
    CellContainer::iterator i = CellMap_.Cells_.find(ci);
    if(i == CellMap_.Cells_.end())
    {
        return;
    }

    switch(i->second.type_)
    {
    case CT_NORMAL:
    case CT_SECRET:
        BuildNormalPolygons(i);
        break;
    case CT_PIT:
        BuildPitPolygons(i);
        break;
    case CT_LIQUID:
        BuildLiquidPolygons(i);
        break;
    case CT_DOOR:
        BuildDoorPolygons(i);
        break;
    case CT_SECRET_DOOR:
        BuildSecretDoorPolygons(i);
        break;
    case CT_TRANSLUCENT_DOOR:
        BuildTranslucentDoorPolygons(i);
        break;
    case CT_TRANSLUCENT_SMALL_WALL:
        BuildTranslucentSmallWallPolygons(i);
        break;
    case CT_TRANSLUCENT_LARGE_WALL:
        BuildTranslucentLargeWallPolygons(i);
        break;
    }
}

void CellMapDocument::BuildNormalPolygons(const CellContainer::iterator& i)
{
    Cell& c = i->second;
    c.polygons_opaque_quads_.clear();
    c.polygons_opaque_tris_.clear();
    c.polygons_translucent_.clear();
    c.polygons_door_.clear();
    c.polygons_door_translucent_.clear();

    Gfx::Polygon p;

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;

    // left
    if(IsSolidSpace(CellIndex(i->first.x_-1, i->first.z_)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // right
    if(IsSolidSpace(CellIndex(i->first.x_+1, i->first.z_)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // front
    if(IsSolidSpace(CellIndex(i->first.x_, i->first.z_+1)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // back
    if(IsSolidSpace(CellIndex(i->first.x_, i->first.z_-1)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // bottom
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
    p.TexColor_     = c.ts_->tex_floor_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_quads_.push_back(p);

    if(c.sky_)
    {
        // left
        if(!IsSky(CellIndex(i->first.x_-1, i->first.z_)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // right
        if(!IsSky(CellIndex(i->first.x_+1, i->first.z_)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // front
        if(!IsSky(CellIndex(i->first.x_, i->first.z_+1)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // back
        if(!IsSky(CellIndex(i->first.x_, i->first.z_-1)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }
    }
    else
    {
        // top
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_ceiling_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }
}

void CellMapDocument::BuildPitPolygons(const CellContainer::iterator& i)
{
    Cell& c = i->second;
    c.polygons_opaque_quads_.clear();
    c.polygons_opaque_tris_.clear();
    c.polygons_translucent_.clear();
    c.polygons_door_.clear();
    c.polygons_door_translucent_.clear();

    Gfx::Polygon p;
    float y0, y1;

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;

    // left
    if(IsSolidSpace(CellIndex(i->first.x_-1, i->first.z_)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // right
    if(IsSolidSpace(CellIndex(i->first.x_+1, i->first.z_)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // front
    if(IsSolidSpace(CellIndex(i->first.x_, i->first.z_+1)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // back
    if(IsSolidSpace(CellIndex(i->first.x_, i->first.z_-1)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    CellContainer::const_iterator itor;
    for(unsigned int j = 0; j < PIT_DEPTH; j++)
    {
        y0 = (float(j) * -cell_size_) - cell_size_;
        y1 = y0 - cell_size_;

        // left
        itor = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_));
        if(itor == CellMap_.Cells_.end() || itor->second.type_ != CT_PIT)
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, y0, max_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, y1, max_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, y1, min_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, y0, min_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j]);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j+1]);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j+1]);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j]);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // right
        itor = CellMap_.Cells_.find(CellIndex(i->first.x_+1, i->first.z_));
        if(itor == CellMap_.Cells_.end() || itor->second.type_ != CT_PIT)
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, y0, min_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, y1, min_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, y1, max_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, y0, max_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j]);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j+1]);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j+1]);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j]);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // front
        itor = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_+1));
        if(itor == CellMap_.Cells_.end() || itor->second.type_ != CT_PIT)
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, y0, max_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, y1, max_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, y1, max_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, y0, max_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j]);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j+1]);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j+1]);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j]);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // back
        itor = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_-1));
        if(itor == CellMap_.Cells_.end() || itor->second.type_ != CT_PIT)
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, y0, min_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, y1, min_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, y1, min_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, y0, min_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j]);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j+1]);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j+1]);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[j]);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }
    }

    y0 = float(PIT_DEPTH) * -cell_size_;
    y1 = y0 - cell_size_;

    // bottom
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, y1, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, y1, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, y1, max_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, y1, min_z));
    p.TexColor_     = c.ts_->tex_floor_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_quads_.push_back(p);

    if(c.sky_)
    {
        // left
        if(!IsSky(CellIndex(i->first.x_-1, i->first.z_)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // right
        if(!IsSky(CellIndex(i->first.x_+1, i->first.z_)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // front
        if(!IsSky(CellIndex(i->first.x_, i->first.z_+1)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // back
        if(!IsSky(CellIndex(i->first.x_, i->first.z_-1)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }
    }
    else
    {
        // top
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_ceiling_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }
}

void CellMapDocument::BuildLiquidPolygons(const CellContainer::iterator& i)
{
    Cell& c = i->second;
    c.polygons_opaque_quads_.clear();
    c.polygons_opaque_tris_.clear();
    c.polygons_translucent_.clear();
    c.polygons_door_.clear();
    c.polygons_door_translucent_.clear();

    Gfx::Polygon p;
    float y0, y1;

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;

    // left
    if(IsSolidSpace(CellIndex(i->first.x_-1, i->first.z_)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // right
    if(IsSolidSpace(CellIndex(i->first.x_+1, i->first.z_)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // front
    if(IsSolidSpace(CellIndex(i->first.x_, i->first.z_+1)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // back
    if(IsSolidSpace(CellIndex(i->first.x_, i->first.z_-1)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    y0 = -cell_size_;
    y1 = y0 - cell_size_;

    // left
    CellContainer::const_iterator itor = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_));
    if(itor == CellMap_.Cells_.end() || itor->second.type_ != CT_LIQUID)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, y0, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, y1, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, y1, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, y0, min_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[1]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[1]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // right
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_+1, i->first.z_));
    if(itor == CellMap_.Cells_.end() || itor->second.type_ != CT_LIQUID)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, y0, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, y1, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, y1, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, y0, max_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[1]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[1]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // front
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_+1));
    if(itor == CellMap_.Cells_.end() || itor->second.type_ != CT_LIQUID)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, y0, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, y1, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, y1, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, y0, max_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[1]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[1]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // back
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_-1));
    if(itor == CellMap_.Cells_.end() || itor->second.type_ != CT_LIQUID)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, y0, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, y1, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, y1, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, y0, min_z));
        p.TexColor_     = c.ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[1]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[1]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), c.ts_->tc_y_[0]);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    y0 = -cell_size_;
    y1 = y0 - cell_size_;

    // bottom
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, y1, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, y1, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, y1, max_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, y1, min_z));
    p.TexColor_     = c.ts_->tex_floor_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_quads_.push_back(p);

    if(c.sky_)
    {
        // left
        if(!IsSky(CellIndex(i->first.x_-1, i->first.z_)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // right
        if(!IsSky(CellIndex(i->first.x_+1, i->first.z_)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // front
        if(!IsSky(CellIndex(i->first.x_, i->first.z_+1)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }

        // back
        if(!IsSky(CellIndex(i->first.x_, i->first.z_-1)))
        {
            p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
            p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
            p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
            p.TexColor_     = c.ts_->tex_wall_;
            p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), 1.0f);
            p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 1.0f-c.ts_->tc_top_y_);
            p.TexLightMap_  = BlankLightMap_;
            SetupPolygonsLightMap(p);
            c.polygons_opaque_quads_.push_back(p);
        }
    }
    else
    {
        // top
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_ceiling_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // The translucent part of the cell.
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_-CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_-CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_-CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_-CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z));
    p.TexColor_     = c.ts_->tex_liquid_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_translucent_.push_back(p);
}

void CellMapDocument::BuildWallPolygonsForInternalCell(const CellContainer::iterator& i)
{
    Cell& c = i->second;
    c.polygons_opaque_quads_.clear();
    c.polygons_opaque_tris_.clear();
    c.polygons_translucent_.clear();
    c.polygons_door_.clear();
    c.polygons_door_translucent_.clear();

    Gfx::Polygon p;

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;

    // left
    if(IsSolidSpace(CellIndex(i->first.x_-1, i->first.z_)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, c.center_z_));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, c.center_z_));
        p.TexColor_     = c.tex_left_bot_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, c.center_z_));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, c.center_z_));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.TexColor_     = c.tex_left_top_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // right
    if(IsSolidSpace(CellIndex(i->first.x_+1, i->first.z_)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, c.center_z_));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, c.center_z_));
        p.TexColor_     = c.tex_right_top_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, c.center_z_));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, c.center_z_));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.TexColor_     = c.tex_right_bot_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // front
    if(IsSolidSpace(CellIndex(i->first.x_, i->first.z_+1)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, max_z));
        p.TexColor_     = c.tex_bot_right_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.TexColor_     = c.tex_bot_left_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // back
    if(IsSolidSpace(CellIndex(i->first.x_, i->first.z_-1)))
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, min_z));
        p.TexColor_     = c.tex_top_left_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.TexColor_     = c.tex_top_right_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), c.tex_left_top_->tc_y_[0]);
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), 0.0f);
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // The floor
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, c.center_z_));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, c.center_z_));
    CellIndex ci(i->first.x_-1, i->first.z_);
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_left_top_->tex_floor_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_floor_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, c.center_z_));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, c.center_z_));
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_left_bot_->tex_floor_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_floor_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, max_z));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, c.center_z_));
    ci = CellIndex(i->first.x_, i->first.z_+1);
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_bot_left_->tex_floor_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_floor_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, c.center_z_));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, max_z));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_bot_right_->tex_floor_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_floor_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, c.center_z_));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, c.center_z_));
    ci = CellIndex(i->first.x_+1, i->first.z_);
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_right_bot_->tex_floor_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_floor_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, c.center_z_));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, c.center_z_));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_right_top_->tex_floor_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_floor_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, c.center_z_));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, min_z));
    ci = CellIndex(i->first.x_, i->first.z_-1);
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_top_right_->tex_floor_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_floor_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, c.center_z_));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(c.center_x_, -cell_size_, min_z));
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_top_left_->tex_floor_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_floor_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    // The ceiling
    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, c.center_z_));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, c.center_z_));
    ci = CellIndex(i->first.x_-1, i->first.z_);
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_left_top_->tex_ceiling_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, c.center_z_));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, c.center_z_));
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_left_bot_->tex_ceiling_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, max_z));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, c.center_z_));
    ci = CellIndex(i->first.x_, i->first.z_+1);
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_bot_left_->tex_ceiling_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, c.center_z_));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, max_z));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_bot_right_->tex_ceiling_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, c.center_z_));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, c.center_z_));
    ci = CellIndex(i->first.x_+1, i->first.z_);
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_right_bot_->tex_ceiling_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, c.center_z_));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, c.center_z_));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_right_top_->tex_ceiling_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, c.center_z_));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, min_z));
    ci = CellIndex(i->first.x_, i->first.z_-1);
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_top_right_->tex_ceiling_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);

    p.Indices_[0]    = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
    p.Indices_[2]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, c.center_z_));
    p.Indices_[1]    = CellMap_.AddPoint(Math::Vector(c.center_x_, 0.0f, min_z));
    if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
    {
        p.TexColor_ = c.tex_top_left_->tex_ceiling_;
    }
    else
    {
        p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_opaque_tris_.push_back(p);
}

void CellMapDocument::BuildDoorPolygons(const CellContainer::iterator& i)
{
    BuildWallPolygonsForInternalCell(i);

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;

    float frame_min_x, frame_max_x;
    float frame_min_z, frame_max_z;

    Gfx::Polygon p;
    Cell& c = i->second;

    // The door frame.

    // Is the door facing left/right, or front/back?
    if(IsSolidSpace(CellIndex(i->first.x_-1, i->first.z_)))
    {
        // Then there is no cell to the left (ie. solid space).  The door must be
        // facing front/back.
        frame_min_z = (min_z+(cell_size_/2.0f))-CellMapDocument::DOOR_FRAME_HALF_THICKNESS;
        frame_max_z = (min_z+(cell_size_/2.0f))+CellMapDocument::DOOR_FRAME_HALF_THICKNESS;

        // Front
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, frame_max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, frame_max_z));
        CellIndex ci(i->first.x_, i->first.z_+1);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, c.center_z_));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, c.center_z_));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_max_z));
        ci = CellIndex(i->first.x_, i->first.z_+1);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_ceiling_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Back
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, frame_min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, frame_min_z));
        ci = CellIndex(i->first.x_, i->first.z_-1);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, c.center_z_));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, c.center_z_));
        ci = CellIndex(i->first.x_, i->first.z_-1);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_ceiling_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }
    else
    {
        frame_min_x = (min_x+(cell_size_/2.0f))-CellMapDocument::DOOR_FRAME_HALF_THICKNESS;
        frame_max_x = (min_x+(cell_size_/2.0f))+CellMapDocument::DOOR_FRAME_HALF_THICKNESS;

        // Left
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(frame_min_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(frame_min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(frame_min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(frame_min_x, 0.0f, max_z));
        CellIndex ci(i->first.x_-1, i->first.z_);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(frame_min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(frame_min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        ci = CellIndex(i->first.x_-1, i->first.z_);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_ceiling_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Right
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(frame_max_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(frame_max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(frame_max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(frame_max_x, 0.0f, min_z));
        ci = CellIndex(i->first.x_+1, i->first.z_);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(frame_max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(frame_max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        ci = CellIndex(i->first.x_+1, i->first.z_);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_ceiling_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }


    // The door itself
    c.door_offset_.x_ = (i->first.x_*cell_size_)+(cell_size_/2.0f);
    c.door_offset_.y_ = 0.0f;
    c.door_offset_.z_ = (i->first.z_*cell_size_)+(cell_size_/2.0f);

    // Is the door facing left/right, or front/back?
    CellContainer::const_iterator itor = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_));
    if(itor == CellMap_.Cells_.end())
    {
        // Then there is no cell to the left (ie. solid space).  The door must be
        // facing front/back.
        min_x = -cell_size_/2.0f;
        max_x = cell_size_/2.0f;
        min_z = -CellMapDocument::DOOR_HALF_THICKNESS;
        max_z = CellMapDocument::DOOR_HALF_THICKNESS;
    }
    else
    {
        min_x = -CellMapDocument::DOOR_HALF_THICKNESS;
        max_x = CellMapDocument::DOOR_HALF_THICKNESS;
        min_z = -cell_size_/2.0f;
        max_z = cell_size_/2.0f;
    }

    // Door top
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
    p.TexColor_     = c.ts_->tex_door_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // Door bottom
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
    p.TexColor_     = c.ts_->tex_door_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // Door left
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
    p.TexColor_     = c.ts_->tex_door_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // Door right
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
    p.TexColor_     = c.ts_->tex_door_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // Door front
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
    p.TexColor_     = c.ts_->tex_door_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // Door back
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
    p.TexColor_     = c.ts_->tex_door_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);
}

void CellMapDocument::BuildSecretDoorPolygons(const CellContainer::iterator& i)
{
    BuildNormalPolygons(i);

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;

    Cell& c = i->second;

    c.door_offset_.x_ = (i->first.x_*cell_size_)+(cell_size_/2.0f);
    c.door_offset_.y_ = 0.0f;
    c.door_offset_.z_ = (i->first.z_*cell_size_)+(cell_size_/2.0f);

    Gfx::Polygon p;

    // right
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, 0.0f, cell_size_/2.0f));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, -cell_size_, cell_size_/2.0f));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, -cell_size_, -cell_size_/2.0f));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, 0.0f, -cell_size_/2.0f));
    CellContainer::iterator itor = CellMap_.Cells_.find(CellIndex(i->first.x_+1, i->first.z_));
    if(itor == CellMap_.Cells_.end())
    {
        p.TexColor_ = c.ts_->tex_wall_;
    }
    else
    {
        p.TexColor_ = itor->second.ts_->tex_wall_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // left
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, 0.0f, -cell_size_/2.0f));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, -cell_size_, -cell_size_/2.0f));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, -cell_size_, cell_size_/2.0f));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, 0.0f, cell_size_/2.0f));
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_));
    if(itor == CellMap_.Cells_.end())
    {
        p.TexColor_ = c.ts_->tex_wall_;
    }
    else
    {
        p.TexColor_ = itor->second.ts_->tex_wall_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // back
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, 0.0f, -cell_size_/2.0f));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, -cell_size_, -cell_size_/2.0f));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, -cell_size_, -cell_size_/2.0f));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, 0.0f, -cell_size_/2.0f));
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_-1));
    if(itor == CellMap_.Cells_.end())
    {
        p.TexColor_ = c.ts_->tex_wall_;
    }
    else
    {
        p.TexColor_ = itor->second.ts_->tex_wall_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // front
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, 0.0f, cell_size_/2.0f));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, -cell_size_, cell_size_/2.0f));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, -cell_size_, cell_size_/2.0f));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, 0.0f, cell_size_/2.0f));
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_+1));
    if(itor == CellMap_.Cells_.end())
    {
        p.TexColor_ = c.ts_->tex_wall_;
    }
    else
    {
        p.TexColor_ = itor->second.ts_->tex_wall_;
    }
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // top
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, 0.0f, -cell_size_/2.0f));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, 0.0f, cell_size_/2.0f));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, 0.0f, cell_size_/2.0f));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, 0.0f, -cell_size_/2.0f));
    p.TexColor_     = c.ts_->tex_wall_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // bottom
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, -cell_size_, cell_size_/2.0f));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(-cell_size_/2.0f, -cell_size_, -cell_size_/2.0f));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, -cell_size_, -cell_size_/2.0f));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(cell_size_/2.0f, -cell_size_, cell_size_/2.0f));
    p.TexColor_     = c.ts_->tex_wall_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);
}

void CellMapDocument::BuildTranslucentDoorPolygons(const CellContainer::iterator& i)
{
    BuildWallPolygonsForInternalCell(i);

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;

    float frame_min_x, frame_max_x;
    float frame_min_z, frame_max_z;
    Gfx::Polygon p;
    Cell& c = i->second;

    // The door frame.

    // Is the door facing left/right, or front/back?
    if(IsSolidSpace(CellIndex(i->first.x_-1, i->first.z_)))
    {
        // Then there is no cell to the left (ie. solid space).  The door must be
        // facing front/back.
        frame_min_z = (min_z+(cell_size_/2.0f))-CellMapDocument::DOOR_FRAME_HALF_THICKNESS;
        frame_max_z = (min_z+(cell_size_/2.0f))+CellMapDocument::DOOR_FRAME_HALF_THICKNESS;

        // Front
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, frame_max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, frame_max_z));
        CellIndex ci(i->first.x_, i->first.z_+1);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, c.center_z_));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, c.center_z_));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_max_z));
        ci = CellIndex(i->first.x_, i->first.z_+1);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_ceiling_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Back
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, frame_min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, frame_min_z));
        ci = CellIndex(i->first.x_, i->first.z_-1);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, c.center_z_));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, frame_min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, c.center_z_));
        ci = CellIndex(i->first.x_, i->first.z_-1);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_ceiling_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }
    else
    {
        frame_min_x = (min_x+(cell_size_/2.0f))-CellMapDocument::DOOR_FRAME_HALF_THICKNESS;
        frame_max_x = (min_x+(cell_size_/2.0f))+CellMapDocument::DOOR_FRAME_HALF_THICKNESS;

        // Left
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(frame_min_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(frame_min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(frame_min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(frame_min_x, 0.0f, max_z));
        CellIndex ci(i->first.x_-1, i->first.z_);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(frame_min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(frame_min_x, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        ci = CellIndex(i->first.x_-1, i->first.z_);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_ceiling_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Right
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(frame_max_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(frame_max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(frame_max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(frame_max_x, 0.0f, min_z));
        ci = CellIndex(i->first.x_+1, i->first.z_);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(frame_max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(frame_max_x, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(c.center_x_, -CellMapDocument::DOOR_FRAME_HEIGHT, max_z));
        ci = CellIndex(i->first.x_+1, i->first.z_);
        if(IsSolidSpace(ci))
        {
            p.TexColor_ = c.ts_->tex_ceiling_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_ceiling_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // The door itself.

    c.door_offset_.x_ = (i->first.x_*cell_size_)+(cell_size_/2.0f);
    c.door_offset_.y_ = 0.0f;
    c.door_offset_.z_ = (i->first.z_*cell_size_)+(cell_size_/2.0f);

    float y0 = 0.0f;
    float y1 = -16.0f;
    float y2 = -cell_size_+16;
    float y3 = -cell_size_;

    // Is the door facing left/right, or front/back?
    CellContainer::iterator itor = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_));
    if(itor == CellMap_.Cells_.end())
    {
        // Then there is no cell to the left (ie. solid space).  The door must be
        // facing front/back.
        float x0 = -cell_size_/2.0f;
        float x1 = (-cell_size_/2.0f)+16.0f;
        float x2 = (cell_size_/2.0f)-16.0f;
        float x3 = cell_size_/2.0f;
        float z0 = -16.0f;
        float z1 = 0.0f;
        float z2 = 16.0f;

        // Front left piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, y0, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, y3, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y2, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, y1, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front top piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, y0, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y1, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y1, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x3, y0, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front right piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x3, y0, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y1, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y2, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x3, y3, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front bottom piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y2, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, y3, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x3, y3, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y2, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back right piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x3, y0, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x3, y3, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y2, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y1, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back top piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x3, y0, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y1, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y1, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, y0, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back left piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y1, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y2, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y3, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, y0, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back bottom piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y2, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x3, y3, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y3, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, y2, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front left sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y1, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y2, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y2, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, y1, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front top sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y1, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y1, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y1, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y1, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front right sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y1, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y2, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y2, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y1, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front bottom sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y2, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y2, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y2, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y2, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back left sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y1, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y2, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y2, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, y1, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back top sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y1, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y1, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y1, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y1, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back right sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y1, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y2, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y2, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y1, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back bottom sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y2, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y2, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y2, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y2, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        min_x = -cell_size_/2.0f;
        max_x = cell_size_/2.0f;
        min_z = (-cell_size_/2.0f+(cell_size_/2.0f)) - CellMapDocument::DOOR_HALF_THICKNESS;
        max_z = (-cell_size_/2.0f+(cell_size_/2.0f)) + CellMapDocument::DOOR_HALF_THICKNESS;

        // Draw the actual door left
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Draw the actual door right
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);
    }
    else
    {
        float x0 = -16.0f;
        float x1 = 0.0f;
        float x2 = 16.0f;
        float z0 = -cell_size_/2.0f;
        float z1 = (-cell_size_/2.0f)+16.0f;
        float z2 = (cell_size_/2.0f)-16.0f;
        float z3 = cell_size_/2.0f;

        // Left back piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y3, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y0, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y1, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y2, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Left top piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y1, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y0, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y0, z3));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y1, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Left front piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y1, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y0, z3));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y3, z3));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y2, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Left bottom piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y3, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y2, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y2, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y3, z3));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Right front piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, y3, z3));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, y0, z3));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y1, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, y2, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Right top piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, y1, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, y0, z3));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y0, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, y1, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Right back piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, y2, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, y1, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y0, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, y3, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Right bottom piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, y3, z3));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, y2, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y2, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, y3, z0));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front left sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y2, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y1, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y1, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, y2, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front top sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y1, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x2, y1, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y1, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, y1, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front right sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y2, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y1, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x2, y1, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y2, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Front bottom sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x2, y2, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y2, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y2, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x2, y2, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back left sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y2, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y1, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y1, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, y2, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back top sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y1, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, y1, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y1, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, y1, z1));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back right sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, y2, z2));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, y1, z2));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, y1, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, y2, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Back bottom sill
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, y2, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, y2, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, y2, z2));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, y2, z2));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        min_x = (-cell_size_/2.0f+(cell_size_/2.0f)) - CellMapDocument::DOOR_HALF_THICKNESS;
        max_x = (-cell_size_/2.0f+(cell_size_/2.0f)) + CellMapDocument::DOOR_HALF_THICKNESS;
        min_z = -cell_size_/2.0f;
        max_z = cell_size_/2.0f;

        // Draw the actual door front
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);

        // Draw the actual door back
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_.push_back(p);
    }

    // Door top
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, max_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, max_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z));
    p.TexColor_     = c.ts_->tex_door_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // Door bottom
    p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, max_z));
    p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z));
    p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z));
    p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, max_z));
    p.TexColor_     = c.ts_->tex_door_;
    p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
    p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
    p.TexLightMap_  = BlankLightMap_;
    SetupPolygonsLightMap(p);
    c.polygons_door_.push_back(p);

    // The translucent part of the door.
    min_x = -cell_size_/2.0f;
    max_x = cell_size_/2.0f;
    min_z = -cell_size_/2.0f;
    max_z = cell_size_/2.0f;

    itor = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_));
    if(itor == CellMap_.Cells_.end())
    {
        // Then there is no cell to the left (ie. solid space).  The door must be
        // facing front/back.
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x+CellMapDocument::SKY_SCHOCHURE_HEIGHT, -CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z+cell_size_/2.0f));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x+CellMapDocument::SKY_SCHOCHURE_HEIGHT, -cell_size_+CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z+cell_size_/2.0f));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x-CellMapDocument::SKY_SCHOCHURE_HEIGHT, -cell_size_+CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z+cell_size_/2.0f));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x-CellMapDocument::SKY_SCHOCHURE_HEIGHT, -CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z+cell_size_/2.0f));
        p.TexColor_     = c.ts_->tex_trans_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_translucent_.push_back(p);
    }
    else
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z+CellMapDocument::SKY_SCHOCHURE_HEIGHT));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -cell_size_+CellMapDocument::SKY_SCHOCHURE_HEIGHT, min_z+CellMapDocument::SKY_SCHOCHURE_HEIGHT));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -cell_size_+CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z-CellMapDocument::SKY_SCHOCHURE_HEIGHT));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -CellMapDocument::SKY_SCHOCHURE_HEIGHT, max_z-CellMapDocument::SKY_SCHOCHURE_HEIGHT));
        p.TexColor_     = c.ts_->tex_trans_door_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_door_translucent_.push_back(p);
    }
}

void CellMapDocument::BuildTranslucentSmallWallPolygons(const CellContainer::iterator& i)
{
    BuildWallPolygonsForInternalCell(i);

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;
    Gfx::Polygon p;
    Cell& c = i->second;

    // Left
    float x0        = min_x;
    float x1        = (min_x+cell_size_/2.0f) - 16.0f;
    float x_half    = min_x+cell_size_/2.0f;
    float z0        = (min_z+cell_size_/2.0f) - 16.0f;
    float z1        = (min_z+cell_size_/2.0f) + 16.0f;
    float z_half    = min_z+cell_size_/2.0f;

    CellIndex ci(i->first.x_-1, i->first.z_);
    if(IsSolidSpace(ci) || IsSameType(ci, CT_TRANSLUCENT_SMALL_WALL))
    {
        // Top piece front
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_left_bot_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_left_bot_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Top piece back
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_left_top_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z_half));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_left_top_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Bottom piece front
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_left_bot_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z_half));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_left_bot_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Bottom piece back
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_left_top_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z_half));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_left_top_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }
    else
    {
        // Top piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z1));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // bottom piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // Right
    x0        = (min_x+cell_size_/2.0f) + 16.0f;
    x1        = max_x;
    x_half    = min_x+cell_size_/2.0f;
    z0        = (min_z+cell_size_/2.0f) - 16.0f;
    z1        = (min_z+cell_size_/2.0f) + 16.0f;
    z_half    = min_z+cell_size_/2.0f;
    ci = CellIndex(i->first.x_+1, i->first.z_);
    if(IsSolidSpace(ci) || IsSameType(ci, CT_TRANSLUCENT_SMALL_WALL))
    {
        // Top piece front
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_right_bot_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z_half));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_right_bot_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Top piece back
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_right_top_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_right_top_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Bottom piece front
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_right_bot_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z_half));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_right_bot_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Bottom piece back
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_right_top_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z_half));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_right_top_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }
    else
    {
        // Top piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z0));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z_half));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // bottom piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // Back
    x0        = (min_x+cell_size_/2.0f) - 16.0f;
    x1        = (min_x+cell_size_/2.0f) + 16.0f;
    x_half    = min_x+cell_size_/2.0f;
    z0        = min_z;
    z1        = (min_z+cell_size_/2.0f) - 16.0f;
    z_half    = min_z+cell_size_/2.0f;
    ci = CellIndex(i->first.x_, i->first.z_-1);
    if(IsSolidSpace(ci) || IsSameType(ci, CT_TRANSLUCENT_SMALL_WALL))
    {
        // Top piece left
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_top_left_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_top_left_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Top piece right
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_top_right_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_top_right_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Bottom piece left
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_top_left_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_top_left_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Bottom piece right
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_top_right_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_top_right_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }
    else
    {
        // Top piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z1));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // bottom piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // Front
    x0        = (min_x+cell_size_/2.0f) - 16.0f;
    x1        = (min_x+cell_size_/2.0f) + 16.0f;
    x_half    = min_x+cell_size_/2.0f;
    z0        = (min_z+cell_size_/2.0f) + 16.0f;
    z1        = max_z;
    z_half    = min_z+cell_size_/2.0f;
    ci = CellIndex(i->first.x_, i->first.z_+1);
    if(IsSolidSpace(ci) || IsSameType(ci, CT_TRANSLUCENT_SMALL_WALL))
    {
        // Top piece left
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_bot_left_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_bot_left_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Top piece right
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_bot_right_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_bot_right_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Bottom piece left
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_bot_left_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z1));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z1));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_bot_left_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // Bottom piece right
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_bot_right_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z1));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z1));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        if(IsSolidSpace(ci) || IsSameType(ci, c.type_))
        {
            p.TexColor_ = c.tex_bot_right_->tex_wall_;
        }
        else
        {
            p.TexColor_ = GetCell(ci).ts_->tex_wall_;
        }
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }
    else
    {
        // Top piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, 0.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, 0.0f, z0));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -32.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z_half));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -32.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x_half, -32.0f, z0));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        // bottom piece
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -cell_size_, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x1, -cell_size_, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);

        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z_half));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(x0, -80.0f, z0));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(x_half, -80.0f, z0));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(x1, -80.0f, z0));
        p.TexColor_     = GetCell(ci).ts_->tex_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_opaque_quads_.push_back(p);
    }

    // The translucent part of the wall.
    // Left
    CellContainer::iterator itor = CellMap_.Cells_.find(CellIndex(i->first.x_-1, i->first.z_));
    if(itor == CellMap_.Cells_.end() || itor->second.type_ == CT_TRANSLUCENT_SMALL_WALL)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, -32.0f, min_z+cell_size_/2.0f));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -80.0f, min_z+cell_size_/2.0f));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -80.0f, min_z+cell_size_/2.0f));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -32.0f, min_z+cell_size_/2.0f));
        p.TexColor_     = c.ts_->tex_trans_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_translucent_.push_back(p);
    }

    // Right
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_+1, i->first.z_));
    if(itor == CellMap_.Cells_.end() || itor->second.type_ == CT_TRANSLUCENT_SMALL_WALL)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x-cell_size_/2.0f, -32.0f, min_z+cell_size_/2.0f));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x-cell_size_/2.0f, -80.0f, min_z+cell_size_/2.0f));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -80.0f, min_z+cell_size_/2.0f));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, -32.0f, min_z+cell_size_/2.0f));
        p.TexColor_     = c.ts_->tex_trans_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_translucent_.push_back(p);
    }

    // Back
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_-1));
    if(itor == CellMap_.Cells_.end() || itor->second.type_ == CT_TRANSLUCENT_SMALL_WALL)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -32.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -80.0f, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -80.0f, min_z+cell_size_/2.0f));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -32.0f, min_z+cell_size_/2.0f));
        p.TexColor_     = c.ts_->tex_trans_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_translucent_.push_back(p);
    }

    // Front
    itor = CellMap_.Cells_.find(CellIndex(i->first.x_, i->first.z_+1));
    if(itor == CellMap_.Cells_.end() || itor->second.type_ == CT_TRANSLUCENT_SMALL_WALL)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -32.0f, max_z-cell_size_/2.0f));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -80.0f, max_z-cell_size_/2.0f));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -80.0f, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -32.0f, max_z));
        p.TexColor_     = c.ts_->tex_trans_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_translucent_.push_back(p);
    }
}

void CellMapDocument::BuildTranslucentLargeWallPolygons(const CellContainer::iterator& i)
{
    BuildWallPolygonsForInternalCell(i);

    float min_x = cell_size_ * float(i->first.x_);
    float max_x = min_x + cell_size_;
    float min_z = cell_size_ * float(i->first.z_);
    float max_z = min_z + cell_size_;

    Gfx::Polygon p;
    Cell& c = i->second;

    // Left
    CellIndex ci(i->first.x_-1, i->first.z_);
    CellContainer::const_iterator itor = CellMap_.Cells_.find(ci);
    if(itor == CellMap_.Cells_.end() || itor->second.type_ == CT_TRANSLUCENT_LARGE_WALL)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x, 0.0f, min_z+cell_size_/2.0f));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x, -cell_size_, min_z+cell_size_/2.0f));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -cell_size_, min_z+cell_size_/2.0f));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, 0.0f, min_z+cell_size_/2.0f));
        p.TexColor_     = c.ts_->tex_trans_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_translucent_.push_back(p);
    }

    // Right
    ci = CellIndex(i->first.x_+1, i->first.z_);
    itor = CellMap_.Cells_.find(ci);
    if(itor == CellMap_.Cells_.end() || itor->second.type_ == CT_TRANSLUCENT_LARGE_WALL)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(max_x-cell_size_/2.0f, 0.0f, min_z+cell_size_/2.0f));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(max_x-cell_size_/2.0f, -cell_size_, min_z+cell_size_/2.0f));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(max_x, -cell_size_, min_z+cell_size_/2.0f));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(max_x, 0.0f, min_z+cell_size_/2.0f));
        p.TexColor_     = c.ts_->tex_trans_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].x_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_translucent_.push_back(p);
    }

    // Back
    ci = CellIndex(i->first.x_, i->first.z_-1);
    itor = CellMap_.Cells_.find(ci);
    if(itor == CellMap_.Cells_.end() || itor->second.type_ == CT_TRANSLUCENT_LARGE_WALL)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, 0.0f, min_z));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -cell_size_, min_z));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -cell_size_, min_z+cell_size_/2.0f));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, 0.0f, min_z+cell_size_/2.0f));
        p.TexColor_     = c.ts_->tex_trans_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_translucent_.push_back(p);
    }

    // Front
    ci = CellIndex(i->first.x_, i->first.z_+1);
    itor = CellMap_.Cells_.find(ci);
    if(itor == CellMap_.Cells_.end() || itor->second.type_ == CT_TRANSLUCENT_LARGE_WALL)
    {
        p.Indices_[0]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, 0.0f, max_z-cell_size_/2.0f));
        p.Indices_[1]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -cell_size_, max_z-cell_size_/2.0f));
        p.Indices_[2]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, -cell_size_, max_z));
        p.Indices_[3]   = CellMap_.AddPoint(Math::Vector(min_x+cell_size_/2.0f, 0.0f, max_z));
        p.TexColor_     = c.ts_->tex_trans_wall_;
        p.TexCoords_[0] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[0]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[0]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[1] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[1]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[1]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[2] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[2]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[2]].y_ / float(p.TexColor_->Height()));
        p.TexCoords_[3] = Gfx::TexCoord(CellMap_.Points_[p.Indices_[3]].z_ / float(p.TexColor_->Width()), CellMap_.Points_[p.Indices_[3]].y_ / float(p.TexColor_->Height()));
        p.TexLightMap_  = BlankLightMap_;
        SetupPolygonsLightMap(p);
        c.polygons_translucent_.push_back(p);
    }
}

void CellMapDocument::CalculateLight(LightList& lights, boost::function<void (int)> total_function, boost::function<void (int)> step_function)
{
    total_function(int(CellMap_.Cells_.size()));

    unsigned long time_prev = timeGetTime();
    CellContainer::iterator i;
    int index = 0;

    for(i = CellMap_.Cells_.begin(); i != CellMap_.Cells_.end(); ++i, ++index)
    {
        CalculateLight_i(i, lights);

        unsigned long now = timeGetTime();
        if(now - time_prev >= 50)
        {
            step_function(index);
            time_prev = now;
        }
    }
}

void CellMapDocument::CalculateLight(const CellIndex& ci, LightList& lights)
{
    CellContainer::iterator i = CellMap_.Cells_.find(ci);
    if(i != CellMap_.Cells_.end())
    {
        CalculateLight_i(i, lights);
    }
}

void CellMapDocument::CalculateLight_i(const CellContainer::iterator& i, LightList& lights)
{
    Cell& c = i->second;
    float ambient;
    if(c.use_world_ambient_light_)
    {
        ambient = float(ambient_) / 100.0f;
    }
    else
    {
        ambient = float(c.ambient_light_) / 100.0f;
    }

    // Go through each point of each polygon and calculate its light value.
    CalculateLight_i(i->first, ambient, c.polygons_opaque_quads_, Math::Vector(), lights);
    CalculateLight_i(i->first, ambient, c.polygons_opaque_tris_, Math::Vector(), lights);
    CalculateLight_i(i->first, ambient, c.polygons_translucent_, Math::Vector(), lights);
    CalculateLight_i(i->first, ambient, c.polygons_door_, c.door_offset_, lights);
    CalculateLight_i(i->first, ambient, c.polygons_door_translucent_, c.door_offset_, lights);
}

void CellMapDocument::CalculateLight_i(const CellIndex& ci,
                                       float ambient,
                                       Gfx::PolygonList& pl,
                                       const Math::Vector& offset,
                                       LightList& lights)
{
    const int Size = LIGHT_MAP_SIZE * LIGHT_MAP_SIZE * 3;
    boost::shared_array<unsigned char> Pixels;
    Pixels.reset(new unsigned char[Size]);

    int YCoord, XCoord;
    Math::Vector PixelWorldCoords;
    float Step, TexCoordU, TexCoordV;

    Step = 1.0f / (float)LIGHT_MAP_SIZE;

    Gfx::PolygonList::iterator PolyItor;
    LightList::iterator LightItor;
    LitPixels::iterator PixItor;
    LitPixels LightPixels;
    LitPixel Pix;
    unsigned int R, G, B;

    for(PolyItor = pl.begin(); PolyItor != pl.end(); ++PolyItor)
    {
        Gfx::Polygon& P = *PolyItor;
        TexCoordU = TexCoordV = 0.0f;

        for(YCoord = 0; YCoord < LIGHT_MAP_SIZE; YCoord++)
        {
            for(XCoord = 0; XCoord < LIGHT_MAP_SIZE; XCoord++)
            {
                // Calc the current point's coordinates in Texture Space
                PixelWorldCoords.x_ = P.ULength_ * TexCoordU;
                PixelWorldCoords.y_ = P.VLength_ * TexCoordV;
                PixelWorldCoords.z_ = 0.0f;

                // Rotate this point into world space
                PixelWorldCoords = P.Matrix_ * PixelWorldCoords;

                // Translate this point in world space
                PixelWorldCoords += CellMap_.Points_[P.Indices_[0]] + offset;

                // Calculate how much light is falling on this pixel
                LightPixels.clear();
                Pix.R_ = (unsigned char)(255.0f * ambient);     // Ambient light is white
                Pix.G_ = (unsigned char)(255.0f * ambient);
                Pix.B_ = (unsigned char)(255.0f * ambient);
                LightPixels.push_back(Pix);

                for(LightItor = lights.begin(); LightItor != lights.end(); ++LightItor)
                {
                    float Distance = fabs(Math::Vector(PixelWorldCoords - LightItor->Position()).Magnitude());
                    if(Distance < LightItor->Radius())
                    {
                        float LightIntensity = 1.0f - (Distance / LightItor->Radius());

                        Pix.R_ = (unsigned char)(255.0f * LightIntensity * LightItor->GetColor().r_);
                        Pix.G_ = (unsigned char)(255.0f * LightIntensity * LightItor->GetColor().g_);
                        Pix.B_ = (unsigned char)(255.0f * LightIntensity * LightItor->GetColor().b_);
                        LightPixels.push_back(Pix);
                    }
                }

                R = G = B = 0;
                for(PixItor = LightPixels.begin(); PixItor != LightPixels.end(); ++PixItor)
                {
                    R += (unsigned int)PixItor->R_;
                    G += (unsigned int)PixItor->G_;
                    B += (unsigned int)PixItor->B_;
                }
                if(R > 255) R = 255;
                if(G > 255) G = 255;
                if(B > 255) B = 255;

                Pixels[YCoord * LIGHT_MAP_SIZE * 3 + XCoord * 3 + 0] = (unsigned char)R;//(R/LightPixels.size());
                Pixels[YCoord * LIGHT_MAP_SIZE * 3 + XCoord * 3 + 1] = (unsigned char)G;//(G/LightPixels.size());
                Pixels[YCoord * LIGHT_MAP_SIZE * 3 + XCoord * 3 + 2] = (unsigned char)B;//(B/LightPixels.size());

                TexCoordU += Step;
            }

            TexCoordV += Step;
            TexCoordU = 0.0f;
        }

        P.TexLightMap_->CreateBilinear(LIGHT_MAP_SIZE, LIGHT_MAP_SIZE, 24, Pixels);
    }
}
