#define NOMINMAX

#include "cellmapwindow.h"
#include "resource.h"
#include "../sagedocuments/cellmapdocument.h"
#include "../sagedocuments/light.h"
#include "../bsp/tree.h"

#include <stdexcept>
#include <algorithm>

#include <commctrl.h>
#include <windowsx.h>
#include <zmouse.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <gl/glext.h>
#include <boost/bind.hpp>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'CellMapWindow *' of greater size

extern PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

const char* CellMapWindow::window_title_ = "CellMapWindow";
const char* CellMapWindow::class_name_   = "CellMapWindow";

CellMapWindow::CellMapWindow(HINSTANCE instance, HWND parent, CellMapWindowEvents* event_handler, Aud::Device* audio_device)
: instance_(instance)
, handle_(NULL)
, map_doc_(NULL)
, event_handler_(event_handler)
, viewport_(0, 0, 100, 100)
, dragging_camera_(false)
, dragging_cursor_(false)
, show_grid_(true)
, anchor_placed_(false)
, first_paint_(true)
, view_style_(VS_TEXTURED_POLYGONS)
, edit_mode_(EM_CELL)
, cell_brush_(CB_SELECT)
, ts_brush_(TSB_SELECT)
, res_cxt_(audio_device)
, sky_brush_(SB_SELECT)
, have_current_light_cell_(false)
, current_light_tool_(LT_SELECTLIGHT)
{
    Register();
    Create(parent);

    RECT rect;
    GetClientRect(handle_, &rect);

    graphics_.Initialise(handle_, float(rect.right), float(rect.bottom), 32, 24);

    viewport_ = Gfx::Viewport(0, 0, rect.right, rect.bottom);
    viewport_.Set();

    graphics_.ClearBackBuffer();
    res_cxt_.CreateDefaultTexture();

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    cell_size_          = 128.0f;
    zoom_x_rotation_    = 45.0f;
    zoom_step_          = cell_size_*3.0f;
    zoom_distance_      = zoom_step_*2.0f;
    max_zoom_distance_  = zoom_step_*10.0f;

    max_x_cells_                = 1000;
    max_z_cells_                = 1000;
    draw_grid_cell_amount_      = 40;
    draw_grid_cell_amount_half_ = 20;

    camera_position_ = Math::Vector(float(max_x_cells_/2)*cell_size_, 0.0f, float(max_z_cells_/2)*cell_size_);
    camera_rotation_ = Math::Vector(0.0f, 0.0f, 0.0f);;

    camera_cell_.x_ = int(camera_position_.x_ / cell_size_);
    camera_cell_.z_ = int(camera_position_.z_ / cell_size_);
}

CellMapWindow::~CellMapWindow()
{
    graphics_.Shutdown();
    Delete();
    Unregister();
}





void CellMapWindow::Register()
{
    WNDCLASS wc;
    wc.style            = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc      = CellMapWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = instance_;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = class_name_;

    if(!RegisterClass(&wc))
    {
        throw std::runtime_error("Unable to register the CellMapWindow's window class");
    }
}

void CellMapWindow::Unregister()
{
    UnregisterClass(class_name_, instance_);
}

void CellMapWindow::Create(HWND parent)
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    window_width_ = parent_client_rect.right-400;
    window_height_ = parent_client_rect.bottom-40;

    handle_ = CreateWindowEx(WS_EX_CLIENTEDGE, class_name_, window_title_, WS_CHILD | WS_VISIBLE,
        200, 40, window_width_, window_height_, parent, NULL, instance_,
        reinterpret_cast<void*>(this));

    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to register CellMapWindow");
    }
}

void CellMapWindow::Delete()
{
    if(handle_)
    {
        DestroyWindow(handle_);
        handle_ = NULL;
    }
}

void CellMapWindow::Repaint()
{
    InvalidateRect(handle_, NULL, FALSE);
}

void CellMapWindow::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    window_width_ = parent_client_rect.right-400;
    window_height_ = parent_client_rect.bottom-40;

    graphics_.Resize(float(window_width_), float(window_height_));
    viewport_ = Gfx::Viewport(0, 0, window_width_, window_height_);
    viewport_.Set();

    SetWindowPos(handle_, NULL, 200, 40, window_width_, window_height_, SWP_NOZORDER);

    Repaint();
}

void CellMapWindow::SetCellMapDocument(CellMapDocument* map_doc)
{
    map_doc_ = map_doc;
    map_doc_->InitialiseDefaultTextureSet(&res_cxt_);
    Repaint();
}

void CellMapWindow::UnloadResourceContext()
{
    res_cxt_.UnloadAll();
}

void CellMapWindow::Enable()
{
    EnableWindow(handle_, TRUE);
}

void CellMapWindow::Disable()
{
    EnableWindow(handle_, FALSE);
}

void CellMapWindow::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void CellMapWindow::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void CellMapWindow::ShowGrid(bool show)
{
    show_grid_ = show;
    Repaint();
}

void CellMapWindow::SetViewStyle(ViewStyle vs)
{
    view_style_ = vs;
    Repaint();
}

void CellMapWindow::Clear()
{
    zoom_distance_      = zoom_step_*2.0f;

    camera_position_ = Math::Vector(float(max_x_cells_/2)*cell_size_, 0.0f, float(max_z_cells_/2)*cell_size_);
    camera_rotation_ = Math::Vector(0.0f, 0.0f, 0.0f);;

    camera_cell_.x_ = int(camera_position_.x_ / cell_size_);
    camera_cell_.z_ = int(camera_position_.z_ / cell_size_);

    map_doc_ = NULL;
    Repaint();
}

void CellMapWindow::SetZoomLevel(int zoom)
{
    zoom_distance_ = zoom_step_ + (float(zoom)*zoom_step_);
    Repaint();
}

void CellMapWindow::SetCellBrush(CellBrush brush)
{
    cell_brush_ = brush;
    if(brush != CB_SELECT)
    {
        if(AreCellsSelected())
        {
            ClearSelectedCells();
        }
    }
}

void CellMapWindow::SetTextureSetBrush(TextureSetBrush ts_brush)
{
    ts_brush_ = ts_brush;
    if(ts_brush != TSB_SELECT)
    {
        if(AreCellsSelected())
        {
            ClearSelectedCells();
        }
    }
}

void CellMapWindow::SetSelectedCellsCellType(CellType cell_type)
{
    CellSelectionList::iterator i;
    for(i = selected_cells_.begin(); i != selected_cells_.end(); ++i)
    {
        event_handler_->OnCellMapWindowInsertCell(*i, cell_type, cell_size_, default_texture_set_);
    }
}

void CellMapWindow::SetSelectedCellsTextureSet(const std::string& ts)
{
    CellSelectionList::iterator i;
    for(i = selected_cells_.begin(); i != selected_cells_.end(); ++i)
    {
        event_handler_->OnCellMapWindowSetTextureSet(*i, ts);
    }
}

void CellMapWindow::DrawCells()
{
    if(map_doc_ == NULL)
    {
        return;
    }

    glEnable(GL_DEPTH_TEST);

    switch(view_style_)
    {
    case VS_WIREFRAME:
        glDisable(GL_CULL_FACE);
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
        map_doc_->DrawCellsOpaque_Colored();
        map_doc_->DrawCellsTranslucent_Textured();
        break;
    case VS_FRONTFACE_WIREFRAME:
        glEnable(GL_CULL_FACE);
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
        map_doc_->DrawCellsOpaque_Colored();
        map_doc_->DrawCellsTranslucent_Textured();
        break;
    case VS_COLORED_POLYGONS:
        glEnable(GL_CULL_FACE);
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
        map_doc_->DrawCellsOpaque_Colored();
        map_doc_->DrawCellsTranslucent_Textured();
        break;
    case VS_TEXTURED_POLYGONS:
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
        map_doc_->DrawCellsOpaque_Textured();
        map_doc_->DrawCellsTranslucent_Textured();
        break;
    }
}

void CellMapWindow::DrawGrid()
{
    glBegin(GL_LINES);

    glColor3f(0.5f, 0.5f, 0.5f);

    for(int z = camera_cell_.z_-draw_grid_cell_amount_half_; z <= camera_cell_.z_+draw_grid_cell_amount_half_; z++)
    {
        glVertex3f(float(camera_cell_.x_-draw_grid_cell_amount_half_)*cell_size_, 0.0f, float(z)*cell_size_);
        glVertex3f(float(camera_cell_.x_+draw_grid_cell_amount_half_)*cell_size_, 0.0f, float(z)*cell_size_);
    }

    for(int x = camera_cell_.x_-draw_grid_cell_amount_half_; x <= camera_cell_.x_+draw_grid_cell_amount_half_; x++)
    {
        glVertex3f(float(x)*cell_size_, 0.0f, float(camera_cell_.z_-draw_grid_cell_amount_half_)*cell_size_);
        glVertex3f(float(x)*cell_size_, 0.0f, float(camera_cell_.z_+draw_grid_cell_amount_half_)*cell_size_);
    }

    glEnd();
}

void CellMapWindow::DrawMouseCursorCellFocus()
{
    // Highlight the cell the cursor is in.
    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
        glVertex3f(float(cursor_cell_.x_)*cell_size_, 0.0f,  float(cursor_cell_.z_)*cell_size_);
        glVertex3f(float(cursor_cell_.x_)*cell_size_, 0.0f,  float(cursor_cell_.z_+1)*cell_size_);
        glVertex3f(float(cursor_cell_.x_+1)*cell_size_, 0.0f,  float(cursor_cell_.z_+1)*cell_size_);
        glVertex3f(float(cursor_cell_.x_+1)*cell_size_, 0.0f,  float(cursor_cell_.z_)*cell_size_);
    glEnd();
    glLineWidth(1.0f);
}

void CellMapWindow::DrawMouseCursorXZIntersection()
{
    // Draw the cursor's intersection point.
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(cursor_intersection_.x_-10.0f, cursor_intersection_.y_-10.0f,  cursor_intersection_.z_-10.0f);
        glVertex3f(cursor_intersection_.x_+10.0f, cursor_intersection_.y_+10.0f,  cursor_intersection_.z_+10.0f);

        glVertex3f(cursor_intersection_.x_-10.0f, cursor_intersection_.y_-10.0f,  cursor_intersection_.z_+10.0f);
        glVertex3f(cursor_intersection_.x_+10.0f, cursor_intersection_.y_+10.0f,  cursor_intersection_.z_-10.0f);

        glVertex3f(cursor_intersection_.x_+10.0f, cursor_intersection_.y_-10.0f,  cursor_intersection_.z_-10.0f);
        glVertex3f(cursor_intersection_.x_-10.0f, cursor_intersection_.y_+10.0f,  cursor_intersection_.z_+10.0f);

        glVertex3f(cursor_intersection_.x_+10.0f, cursor_intersection_.y_-10.0f,  cursor_intersection_.z_+10.0f);
        glVertex3f(cursor_intersection_.x_-10.0f, cursor_intersection_.y_+10.0f,  cursor_intersection_.z_-10.0f);
    glEnd();
}

void CellMapWindow::DrawSelectedCells()
{
    glEnable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 0.0f, 0.75f);
    glBegin(GL_QUADS);
    CellSelectionList::iterator i;
    for(i = selected_cells_.begin(); i != selected_cells_.end(); ++i)
    {
        glVertex3f(float(i->x_)*cell_size_, 0.0f,  float(i->z_)*cell_size_);
        glVertex3f(float(i->x_)*cell_size_, 0.0f,  float(i->z_+1)*cell_size_);
        glVertex3f(float(i->x_+1)*cell_size_, 0.0f,  float(i->z_+1)*cell_size_);
        glVertex3f(float(i->x_+1)*cell_size_, 0.0f,  float(i->z_)*cell_size_);
    }
    glEnd();
    glDisable(GL_BLEND);
}

void CellMapWindow::DrawSkyCellsAsTextures()
{
    const CellContainer& Cells = map_doc_->GetCellMap().Cells_;

    if(sky_texture_)
    {
        glBindTexture(GL_TEXTURE_2D, sky_texture_->Id());

        glEnable(GL_BLEND);
        glBegin(GL_QUADS);
        glColor4f(1.0f, 1.0f, 1.0f, 0.75f);

        CellContainer::const_iterator i;
        for(i = Cells.begin(); i != Cells.end(); ++i)
        {
            if(i->second.sky_)
            {
                float min_x = float(i->first.x_)*cell_size_;
                float max_x = float(i->first.x_+1)*cell_size_;
                float min_z = float(i->first.z_)*cell_size_;
                float max_z = float(i->first.z_+1)*cell_size_;

                glTexCoord2f(0.0f, 0.0f); glVertex3f(min_x, 0.0f, min_z);
                glTexCoord2f(0.0f, 1.0f); glVertex3f(min_x, 0.0f, max_z);
                glTexCoord2f(1.0f, 1.0f); glVertex3f(max_x, 0.0f, max_z);
                glTexCoord2f(1.0f, 0.0f); glVertex3f(max_x, 0.0f, min_z);
            }
        }

        glEnd();
        glDisable(GL_BLEND);
    }
}

void CellMapWindow::DrawSkyCellsAsLines()
{
    const CellContainer& Cells = map_doc_->GetCellMap().Cells_;

    glPolygonMode(GL_FRONT, GL_LINE);
    glLineWidth(3.0f);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);

    CellContainer::const_iterator i;
    for(i = Cells.begin(); i != Cells.end(); ++i)
    {
        if(i->second.sky_)
        {
            glVertex3f(float(i->first.x_)*cell_size_, 0.0f,  float(i->first.z_)*cell_size_);
            glVertex3f(float(i->first.x_)*cell_size_, 0.0f,  float(i->first.z_+1)*cell_size_);
            glVertex3f(float(i->first.x_+1)*cell_size_, 0.0f,  float(i->first.z_+1)*cell_size_);
            glVertex3f(float(i->first.x_+1)*cell_size_, 0.0f,  float(i->first.z_)*cell_size_);
        }
    }

    glEnd();
    glPolygonMode(GL_FRONT, GL_FILL);
    glLineWidth(1.0f);
}

void CellMapWindow::DrawLights()
{
    LightList& ll = map_doc_->GetLightList();
    LightList::iterator i;

    // Draw the lights
    glColor3f(1.0f, 1.0f, 0.0f);
    for(i = ll.begin(); i != ll.end(); ++i)
    {
        glPushMatrix();
        glTranslatef(i->Position().x_, i->Position().y_, i->Position().z_);
        glBegin(GL_QUADS);
            glVertex3f(-5.0f,  5.0f,  5.0f);     // Front
            glVertex3f(-5.0f, -5.0f,  5.0f);
            glVertex3f( 5.0f, -5.0f,  5.0f);
            glVertex3f( 5.0f,  5.0f,  5.0f);

            glVertex3f(-5.0f,  5.0f, -5.0f);     // Top
            glVertex3f(-5.0f,  5.0f,  5.0f);
            glVertex3f( 5.0f,  5.0f,  5.0f);
            glVertex3f( 5.0f,  5.0f, -5.0f);

            glVertex3f(-5.0f,  5.0f, -5.0f);     // Left
            glVertex3f(-5.0f, -5.0f, -5.0f);
            glVertex3f(-5.0f, -5.0f,  5.0f);
            glVertex3f(-5.0f,  5.0f,  5.0f);

            glVertex3f( 5.0f,  5.0f, -5.0f);     // Back
            glVertex3f( 5.0f, -5.0f, -5.0f);
            glVertex3f(-5.0f, -5.0f, -5.0f);
            glVertex3f(-5.0f,  5.0f, -5.0f);

            glVertex3f(-5.0f, -5.0f,  5.0f);     // Bottom
            glVertex3f(-5.0f, -5.0f, -5.0f);
            glVertex3f( 5.0f, -5.0f, -5.0f);
            glVertex3f( 5.0f, -5.0f,  5.0f);

            glVertex3f( 5.0f,  5.0f,  5.0f);     // Right
            glVertex3f( 5.0f, -5.0f,  5.0f);
            glVertex3f( 5.0f, -5.0f, -5.0f);
            glVertex3f( 5.0f,  5.0f, -5.0f);
        glEnd();
        glPopMatrix();
    }

    LightSelectionList::iterator l;
    if(current_light_tool_ == LT_LIGHTRADIUS)
    {
        // Note: At this point the drawing color is Yellow
        for(l = selected_lights_.begin(); l != selected_lights_.end(); ++l)
        {
            glPushMatrix();
            glTranslatef(l->light_->Position().x_, l->light_->Position().y_, l->light_->Position().z_);
            glutWireSphere(l->light_->Radius(), 16, 16);
            glPopMatrix();
        }
    }

    // Draw the selection over the top of the lights.
    glColor3f(1.0f, 0.5f, 0.0f);
    for(l = selected_lights_.begin(); l != selected_lights_.end(); ++l)
    {
        glPushMatrix();
        glTranslatef(l->light_->Position().x_, l->light_->Position().y_, l->light_->Position().z_);
        glBegin(GL_QUADS);
            glVertex3f(-10.0f,  10.0f,  10.0f);     // Front
            glVertex3f(-10.0f, -10.0f,  10.0f);
            glVertex3f( 10.0f, -10.0f,  10.0f);
            glVertex3f( 10.0f,  10.0f,  10.0f);

            glVertex3f(-10.0f,  10.0f, -10.0f);     // Top
            glVertex3f(-10.0f,  10.0f,  10.0f);
            glVertex3f( 10.0f,  10.0f,  10.0f);
            glVertex3f( 10.0f,  10.0f, -10.0f);

            glVertex3f(-10.0f,  10.0f, -10.0f);     // Left
            glVertex3f(-10.0f, -10.0f, -10.0f);
            glVertex3f(-10.0f, -10.0f,  10.0f);
            glVertex3f(-10.0f,  10.0f,  10.0f);

            glVertex3f( 10.0f,  10.0f, -10.0f);     // Back
            glVertex3f( 10.0f, -10.0f, -10.0f);
            glVertex3f(-10.0f, -10.0f, -10.0f);
            glVertex3f(-10.0f,  10.0f, -10.0f);

            glVertex3f(-10.0f, -10.0f,  10.0f);     // Bottom
            glVertex3f(-10.0f, -10.0f, -10.0f);
            glVertex3f( 10.0f, -10.0f, -10.0f);
            glVertex3f( 10.0f, -10.0f,  10.0f);

            glVertex3f( 10.0f,  10.0f,  10.0f);     // Right
            glVertex3f( 10.0f, -10.0f,  10.0f);
            glVertex3f( 10.0f, -10.0f, -10.0f);
            glVertex3f( 10.0f,  10.0f, -10.0f);
        glEnd();
        glPopMatrix();
    }
}

CellType CellMapWindow::ToCellType(CellBrush brush) const
{
    switch(brush)
    {
    case CB_NORMAL: return CT_NORMAL;
    case CB_SECRET: return CT_SECRET;
    case CB_PIT: return CT_PIT;
    case CB_LIQUID: return CT_LIQUID;
    case CB_DOOR: return CT_DOOR;
    case CB_SECRET_DOOR: return CT_SECRET_DOOR;
    case CB_TRANSLUCENT_DOOR: return CT_TRANSLUCENT_DOOR;
    case CB_TRANSLUCENT_SMALL_WALL: return CT_TRANSLUCENT_SMALL_WALL;
    case CB_TRANSLUCENT_LARGE_WALL: return CT_TRANSLUCENT_LARGE_WALL;
    }
    return CT_SOLID_SPACE;
}

void CellMapWindow::SetupModelViewTransform()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // This is essentially a '3rd person' style transformation
    glTranslatef(0.0f, 0.0f, -zoom_distance_);
    glRotatef(zoom_x_rotation_, 1.0f, 0.0f, 0.0f );

    glRotatef(-camera_rotation_.x_, 1.0f, 0.0f, 0.0f );
    glRotatef(-camera_rotation_.y_, 0.0f, 1.0f, 0.0f );
    glTranslatef(-camera_position_.x_, -camera_position_.y_, -camera_position_.z_);
}

Math::Vector CellMapWindow::WindowToWorldCoords(int client_x, int client_y)
{
    client_y = window_height_ - client_y;

    glPushMatrix();
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        GLdouble model_view_matrix[16];
        SetupModelViewTransform();
        glGetDoublev(GL_MODELVIEW_MATRIX, model_view_matrix);

        GLdouble projection_matrix[16];
        graphics_.GetPerspectiveProjectionMatrix(projection_matrix);

        GLdouble x, y, z;
        gluUnProject(
            GLdouble(client_x), GLdouble(client_y),
            1.0, model_view_matrix, projection_matrix,
            viewport, &x, &y, &z);

        glMatrixMode(GL_MODELVIEW);     // Set the current mode back to this.
    glPopMatrix();

    Math::Plane xz_plane(Math::Vector(0.0f, 1.0f, 0.0f), Math::Vector(0.0f, 0.0f, 0.0f));
    Math::Vector pos(camera_position_);

    float distance = zoom_distance_ * cos(zoom_x_rotation_*Math::DTOR);
    pos.x_ += distance * sin(camera_rotation_.y_*Math::DTOR);
    pos.z_ += distance * cos(camera_rotation_.y_*Math::DTOR);

    pos.y_ += zoom_distance_ * sin(zoom_x_rotation_*Math::DTOR);
    Math::LineSegment line_segment(pos, Math::Vector(float(x), float(y), float(z)));

    Math::Vector intersection;
    float percent;
    xz_plane.GetIntersection(line_segment, intersection, 0.0f, &percent);

    return intersection;
}

Math::LineSegment CellMapWindow::MouseCursorWorldLineSegment(int client_x, int client_y)
{
    client_y = window_height_ - client_y;

    glPushMatrix();
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        GLdouble model_view_matrix[16];
        SetupModelViewTransform();
        glGetDoublev(GL_MODELVIEW_MATRIX, model_view_matrix);

        GLdouble projection_matrix[16];
        graphics_.GetPerspectiveProjectionMatrix(projection_matrix);

        GLdouble x, y, z;
        gluUnProject(
            GLdouble(client_x), GLdouble(client_y),
            1.0, model_view_matrix, projection_matrix,
            viewport, &x, &y, &z);

        glMatrixMode(GL_MODELVIEW);     // Set the current mode back to this.
    glPopMatrix();

    Math::Plane xz_plane(Math::Vector(0.0f, 1.0f, 0.0f), Math::Vector(0.0f, 0.0f, 0.0f));
    Math::Vector pos(camera_position_);

    float distance = zoom_distance_ * cos(zoom_x_rotation_*Math::DTOR);
    pos.x_ += distance * sin(camera_rotation_.y_*Math::DTOR);
    pos.z_ += distance * cos(camera_rotation_.y_*Math::DTOR);

    pos.y_ += zoom_distance_ * sin(zoom_x_rotation_*Math::DTOR);
    return Math::LineSegment(pos, Math::Vector(float(x), float(y), float(z)));
}


void CellMapWindow::AddSelectedCell(const CellIndex& c)
{
    CellSelectionList::iterator i = std::find(selected_cells_.begin(), selected_cells_.end(), c);
    if(i == selected_cells_.end())
    {
        selected_cells_.push_back(c);
    }
}

void CellMapWindow::RemoveSelectedCell(const CellIndex& c)
{
    CellSelectionList::iterator i = std::find(selected_cells_.begin(), selected_cells_.end(), c);
    if(i != selected_cells_.end())
    {
        selected_cells_.erase(i);
    }
}

bool CellMapWindow::IsCellSelected(const CellIndex& c) const
{
    CellSelectionList::const_iterator i = std::find(selected_cells_.begin(), selected_cells_.end(), c);
    return i != selected_cells_.end();
}





void CellMapWindow::SetSkyBrush(SkyBrush sky_brush)
{
    sky_brush_ = sky_brush;
    if(sky_brush_ != SB_SELECT)
    {
        ClearSelectedCells();
        Repaint();
    }
}

void CellMapWindow::SetSelectedCellsToSky()
{
    CellSelectionList::iterator i;
    for(i = selected_cells_.begin(); i != selected_cells_.end(); ++i)
    {
        event_handler_->OnCellMapWindowSetSkyFlag(*i, true);
    }
    Repaint();
}

void CellMapWindow::ClearSelectedCellsSky()
{
    CellSelectionList::iterator i;
    for(i = selected_cells_.begin(); i != selected_cells_.end(); ++i)
    {
        event_handler_->OnCellMapWindowSetSkyFlag(*i, false);
    }
    Repaint();
}

void CellMapWindow::SkyTextureFilename(const std::string& content_dir, const std::string& filename)
{
    if(!filename.empty())
    {
        sky_texture_ = res_cxt_.LoadTexture(content_dir + "\\Textures\\" + filename);
    }
}

void CellMapWindow::SelectLights(const std::vector<std::string>& light_names)
{
    LightList& ll = map_doc_->GetLightList();
    ClearSelectedLights();

    std::vector<std::string>::const_iterator light_itor;
    for(light_itor = light_names.begin(); light_itor != light_names.end(); ++light_itor)
    {
        LightList::iterator i = std::find(ll.begin(), ll.end(), *light_itor);
        if(i != ll.end())
        {
            SelectLight(&(*i));
        }
    }
    Repaint();
}

void CellMapWindow::SetCurrentLightTool(LightTool tool)
{
    current_light_tool_ = tool;
    ClearSelectedLights();
    Repaint();
}

void CellMapWindow::InsertLight(const std::string& name)
{
    Light l;
    l.Name(name);
    l.SetCellIndex(camera_cell_);
    l.Position(camera_position_, map_doc_->CellSize());
    l.Radius(256.0f);
    l.UseWorldAmbient(true);
    l.Ambient(25);

    LightList& ll = map_doc_->GetLightList();
    ll.push_back(l);

    //map_doc_->CalculateLight(
    //    CellIndex(l.Position().x_/map_doc_->CellSize(), l.Position().z_/map_doc_->CellSize()),
    //    map_doc_->GetLightList());

    Repaint();
}

void CellMapWindow::RemoveLights(const std::vector<std::string>& light_names)
{
    LightList& ll = map_doc_->GetLightList();

    std::vector<std::string>::const_iterator itor;
    for(itor = light_names.begin(); itor != light_names.end(); ++itor)
    {
        LightList::iterator i = std::find(ll.begin(), ll.end(), *itor);
        if(i != ll.end())
        {
            ll.erase(i);
        }
    }

    //for(itor = light_names.begin(); itor != light_names.end(); ++itor)
    //{
    //    LightList::iterator i = std::find(ll.begin(), ll.end(), *itor);
    //    if(i != ll.end())
    //    {
    //        map_doc_->CalculateLight(
    //            CellIndex(i->Position().x_/map_doc_->CellSize(), i->Position().z_/map_doc_->CellSize()),
    //            map_doc_->GetLightList());
    //    }
    //}

    Repaint();
}

void CellMapWindow::SetLightRadius(const std::vector<std::string>& light_names, int radius)
{
    LightList& ll = map_doc_->GetLightList();

    std::vector<std::string>::const_iterator itor;
    for(itor = light_names.begin(); itor != light_names.end(); ++itor)
    {
        LightList::iterator i = std::find(ll.begin(), ll.end(), *itor);
        if(i != ll.end())
        {
            i->Radius(float(radius));
        }
    }

    //for(itor = light_names.begin(); itor != light_names.end(); ++itor)
    //{
    //    LightList::iterator i = std::find(ll.begin(), ll.end(), *itor);
    //    if(i != ll.end())
    //    {
    //        map_doc_->CalculateLight(
    //            CellIndex(i->Position().x_/map_doc_->CellSize(), i->Position().z_/map_doc_->CellSize()),
    //            map_doc_->GetLightList());
    //    }
    //}

    Repaint();
}

void CellMapWindow::CurrentLightCellSetUseWorldAmbient()
{
    if(have_current_light_cell_)
    {
        CellContainer& Cells = map_doc_->GetCellMap().Cells_;
        CellContainer::iterator i = Cells.find(current_light_cell_);
        if(i != Cells.end())
        {
            i->second.use_world_ambient_light_ = true;
            Repaint();
        }
    }
}

void CellMapWindow::CurrentLightCellSetSpecifyAmbient()
{
    if(have_current_light_cell_)
    {
        CellContainer& Cells = map_doc_->GetCellMap().Cells_;
        CellContainer::iterator i = Cells.find(current_light_cell_);
        if(i != Cells.end())
        {
            i->second.use_world_ambient_light_ = false;
            Repaint();
        }
    }
}

void CellMapWindow::CurrentLightCellSetAmbientSpecificLight(int ambient)
{
    if(have_current_light_cell_)
    {
        CellContainer& Cells = map_doc_->GetCellMap().Cells_;
        CellContainer::iterator i = Cells.find(current_light_cell_);
        if(i != Cells.end())
        {
            i->second.use_world_ambient_light_  = false;
            i->second.ambient_light_            = ambient;
            Repaint();
        }
    }
}

void CellMapWindow::SetAmbientWorldLight(int ambient)
{
    map_doc_->AmbientLight(ambient);
    //map_doc_->CalculateLight(map_doc_->GetLightList(), boost::bind(&CellMapWindow::OnCalculatingLightingTotal, this, _1),
    //    boost::bind(&CellMapWindow::OnCalculatingLightingStep, this, _1));
    Repaint();
}

void CellMapWindow::SetLightsColor(const std::vector<std::string>& light_names, DWORD color)
{
    // TODO: ...
}

void CellMapWindow::OnCalculatingLightingTotal(int total)
{
}

void CellMapWindow::OnCalculatingLightingStep(int value)
{
}

void CellMapWindow::ViewLight(const std::string& light_name)
{
    LightList& ll = map_doc_->GetLightList();
    LightList::iterator i = std::find(ll.begin(), ll.end(), light_name);
    if(i != ll.end())
    {
        camera_position_        = i->Position();
        camera_position_.y_     = 0.0f;
        camera_cell_.x_         = int(camera_position_.x_ / cell_size_);
        camera_cell_.z_         = int(camera_position_.z_ / cell_size_);
        Repaint();
    }
}




LRESULT CALLBACK CellMapWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(window, GWL_USERDATA);
    CellMapWindow* this_ = reinterpret_cast<CellMapWindow*>(data);

    switch(msg)
    {
    case WM_CREATE:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(window, GWL_USERDATA, reinterpret_cast<LONG>(create_struct->lpCreateParams));
            this_ = reinterpret_cast<CellMapWindow*>(create_struct->lpCreateParams);
            return 0;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(window, &ps);
            this_->OnWindowPaint(dc);
            EndPaint(window, &ps);
            return 0;
        }
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnLmButtonDown(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            return 0;
        }
    case WM_LBUTTONUP:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnLmButtonUp(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            return 0;
        }
    case WM_LBUTTONDBLCLK:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnLmButtonDblClk(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            return 0;
        }
    case WM_RBUTTONDOWN:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnRmButtonDown(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            return 0;
        }
    case WM_RBUTTONUP:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnRmButtonUp(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            return 0;
        }
    case WM_RBUTTONDBLCLK:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnRmButtonDblClk(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            return 0;
        }
    case WM_MOUSEMOVE:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnMouseMove(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            return 0;
        }
    case WM_MOUSEWHEEL:
        short(HIWORD(wparam)) < 0 ? this_->OnMouseWheelRotatedBackwards() : this_->OnMouseWheelRotatedForwards();
        return 0;
    }

    return DefWindowProc(window, msg, wparam, lparam);
}

void CellMapWindow::OnWindowPaint(HDC dc)
{
    graphics_.ClearBackBuffer();
    graphics_.Set3d();

    SetupModelViewTransform();

    DrawCells();

    if(edit_mode_ == EM_SKY)
    {
        DrawSkyCellsAsTextures();
    }

    glActiveTextureARB(GL_TEXTURE0_ARB); glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB); glDisable(GL_TEXTURE_2D);
    DrawLights();

    glDisable(GL_DEPTH_TEST);

    if(edit_mode_ != EM_SKY)
    {
        DrawSkyCellsAsLines();
    }

    DrawSelectedCells();
    if(show_grid_) DrawGrid();
    if(edit_mode_ != EM_LIGHT || (edit_mode_ == EM_LIGHT && current_light_tool_ == LT_CELLAMBIENCE))
    {
        DrawMouseCursorCellFocus();
    }
    DrawMouseCursorXZIntersection();

    graphics_.PageFlip();

    if(first_paint_)
    {
        first_paint_ = false;
        OnWindowPaint(dc);
    }
}

void CellMapWindow::OnLmButtonDown(const POINT& point, bool control_held, bool shift_held)
{
    SetFocus(handle_);
    SetCapture(handle_);

    dragging_cursor_ = true;
    prev_point_ = point;

    switch(edit_mode_)
    {
    case EM_CELL:
        {
            switch(cell_brush_)
            {
            case CB_SELECT:
                DoBeginSelectionBlockDrag(point, control_held, shift_held);
                break;
            case CB_SOLID_SPACE:
                event_handler_->OnCellMapWindowRemoveCell(cursor_cell_);
                break;
            default:
                event_handler_->OnCellMapWindowInsertCell(cursor_cell_, ToCellType(cell_brush_), cell_size_, default_texture_set_);
                break;
            }
            break;
        }
    case EM_TEXTURE_SET:
        {
            switch(ts_brush_)
            {
            case TSB_SELECT:
                DoBeginSelectionBlockDrag(point, control_held, shift_held);
                break;
            case TSB_APPLY:
                event_handler_->OnCellMapWindowSetTextureSet(cursor_cell_, texture_set_);
                break;
            }
            break;
        }
    case EM_SOUND:
        break;
    case EM_LIGHT:
        switch(current_light_tool_)
        {
        case LT_SELECTLIGHT:
            {
                Light* l = LineToLightIntersection(point);
                if(l)
                {
                    if(shift_held)
                    {
                        SelectLight(l);
                    }
                    else if(control_held)
                    {
                        if(IsLightSelected(l))
                        {
                            DeselectLight(l);
                        }
                        else
                        {
                            SelectLight(l);
                        }
                    }
                    else
                    {
                        ClearSelectedLights();
                        SelectLight(l);
                    }
                }
                else
                {
                    if(!control_held && !shift_held)
                    {
                        ClearSelectedLights();
                    }
                }

                Math::Vector world_pos = WindowToWorldCoords(point.x, point.y);

                LightSelectionList::iterator itor;
                for(itor = selected_lights_.begin(); itor != selected_lights_.end(); ++itor)
                {
                    itor->drag_offset_ = world_pos - itor->light_->Position();
                }
                break;
            }
        case LT_CELLAMBIENCE:
            break;
        case LT_LIGHTRADIUS:
            {
                Light* l = LineToLightIntersection(point);
                if(l)
                {
                    if(shift_held)
                    {
                        SelectLight(l);
                    }
                    else if(control_held)
                    {
                        if(IsLightSelected(l))
                        {
                            DeselectLight(l);
                        }
                        else
                        {
                            SelectLight(l);
                        }
                    }
                    else
                    {
                        ClearSelectedLights();
                        SelectLight(l);
                    }
                }
                break;
            }
        }
        break;
    case EM_ENTITY:
        break;
    case EM_CAMERA:
        break;
    case EM_REGION:
        break;
    case EM_FOG:
        break;
    case EM_TRIGGER:
        break;
    case EM_SKY:
        {
            switch(sky_brush_)
            {
            case SB_SELECT:
                DoBeginSelectionBlockDrag(point, control_held, shift_held);
                break;
            case SB_SETSKY:
                event_handler_->OnCellMapWindowSetSkyFlag(cursor_cell_, true);
                break;
            case SB_CLEARSKY:
                event_handler_->OnCellMapWindowSetSkyFlag(cursor_cell_, false);
                break;
            }
            break;
        }
    }

    Repaint();
}

void CellMapWindow::OnLmButtonUp(const POINT& point, bool control_held, bool shift_held)
{
    if(handle_ == GetCapture())
    {
        ReleaseCapture();
        dragging_cursor_ = false;
    }
}

void CellMapWindow::OnLmButtonDblClk(const POINT& point, bool control_held, bool shift_held)
{
}

void CellMapWindow::OnRmButtonDown(const POINT& point, bool control_held, bool shift_held)
{
    SetFocus(handle_);
    SetCapture(handle_);

    dragging_camera_ = true;
    prev_mouse_ = point;
}

void CellMapWindow::OnRmButtonUp(const POINT& point, bool control_held, bool shift_held)
{
    if(handle_ == GetCapture())
    {
        dragging_camera_ = false;
        ReleaseCapture();
    }
}

void CellMapWindow::OnRmButtonDblClk(const POINT& point, bool control_held, bool shift_held)
{
    cursor_intersection_    = WindowToWorldCoords(point.x, point.y);
    camera_position_        = cursor_intersection_;
    camera_cell_.x_         = int(camera_position_.x_ / cell_size_);
    camera_cell_.z_         = int(camera_position_.z_ / cell_size_);
    Repaint();
}

void CellMapWindow::OnMouseMove(const POINT& point, bool control_held, bool shift_held)
{
    Math::Vector world_pos = WindowToWorldCoords(point.x, point.y);

    bool repaint = false;
    if(dragging_camera_)
    {
        if(shift_held)
        {
            camera_rotation_.y_ += float(point.x - prev_mouse_.x);
        }
        else
        {
            Math::Vector prev_position = WindowToWorldCoords(prev_mouse_.x, prev_mouse_.y);

            camera_position_ -= (world_pos - prev_position);
            camera_cell_.x_ = int(camera_position_.x_ / cell_size_);
            camera_cell_.z_ = int(camera_position_.z_ / cell_size_);
        }

        prev_mouse_.x = point.x;
        prev_mouse_.y = point.y;

        repaint = true;
    }
    else
    {
        cursor_intersection_ = world_pos;

        cursor_cell_.x_ = int(cursor_intersection_.x_ / cell_size_);
        cursor_cell_.z_ = int(cursor_intersection_.z_ / cell_size_);

        repaint = true;
    }

    if(handle_ == GetCapture() && dragging_cursor_)
    {
        switch(edit_mode_)
        {
        case EM_CELL:
            {
                switch(cell_brush_)
                {
                case CB_SELECT:
                    DoSelectionBlockDrag(point, control_held, shift_held);
                    repaint = true;
                    break;
                case CB_SOLID_SPACE:
                    event_handler_->OnCellMapWindowRemoveCell(cursor_cell_);
                    break;
                default:
                    event_handler_->OnCellMapWindowInsertCell(cursor_cell_, ToCellType(cell_brush_), cell_size_, default_texture_set_);
                    break;
                }
                break;
            }
        case EM_TEXTURE_SET:
            {
                switch(ts_brush_)
                {
                case TSB_SELECT:
                    DoSelectionBlockDrag(point, control_held, shift_held);
                    repaint = true;
                    break;
                case TSB_APPLY:
                    event_handler_->OnCellMapWindowSetTextureSet(cursor_cell_, texture_set_);
                    break;
                }
                break;
            }
        case EM_SOUND:
            break;
        case EM_LIGHT:
            switch(current_light_tool_)
            {
            case LT_SELECTLIGHT:
                {
                    std::vector<Light*> lights;
                    if(shift_held)
                    {
                        float delta = float(prev_point_.y - point.y);
                        Math::Vector v;
                        LightSelectionList::iterator itor;
                        for(itor = selected_lights_.begin(); itor != selected_lights_.end(); ++itor)
                        {
                            v = itor->light_->Position();
                            v.y_ += delta;
                            itor->light_->Position(v, cell_size_);
                            lights.push_back(itor->light_);
                        }
                    }
                    else
                    {
                        LightSelectionList::iterator itor;
                        for(itor = selected_lights_.begin(); itor != selected_lights_.end(); ++itor)
                        {
                            itor->light_->Position(world_pos - itor->drag_offset_, cell_size_);
                            lights.push_back(itor->light_);
                        }
                    }
                    if(!selected_lights_.empty())
                    {
                        event_handler_->OnCellMapLightPositionChanged(lights);
                    }
                    break;
                }
            case LT_CELLAMBIENCE:
                break;
            case LT_LIGHTRADIUS:
                {
                    std::vector<Light*> lights;
                    float delta = float(point.x - prev_point_.x);
                    float temp;
                    if(shift_held) delta *= 2.0f;
                    LightSelectionList::iterator l;
                    for(l = selected_lights_.begin(); l != selected_lights_.end(); ++l)
                    {
                        temp = l->light_->Radius() + delta;
                        if(temp < 10.0f) temp = 10.0f;
                        l->light_->Radius(temp);
                        lights.push_back(l->light_);
                    }
                    event_handler_->OnCellMapLightRadiusChanged(lights);
                    break;
                }
            }
            break;
        case EM_ENTITY:
            break;
        case EM_CAMERA:
            break;
        case EM_REGION:
            break;
        case EM_FOG:
            break;
        case EM_TRIGGER:
            break;
        case EM_SKY:
            {
                switch(sky_brush_)
                {
                case SB_SELECT:
                    DoSelectionBlockDrag(point, control_held, shift_held);
                    repaint = true;
                    break;
                case SB_SETSKY:
                    event_handler_->OnCellMapWindowSetSkyFlag(cursor_cell_, true);
                    break;
                case SB_CLEARSKY:
                    event_handler_->OnCellMapWindowSetSkyFlag(cursor_cell_, false);
                    break;
                }
                break;
            }
        }
    }

    prev_point_ = point;
    if(repaint)
    {
        Repaint();
    }
}

void CellMapWindow::OnMouseWheelRotatedForwards()
{
    if(zoom_distance_ - zoom_step_ >= zoom_step_)
    {
        zoom_distance_ -= zoom_step_;
        Repaint();
        event_handler_->OnCellMapWindowZoomLevelChanged(int(zoom_distance_/zoom_step_)-1);
    }
}

void CellMapWindow::OnMouseWheelRotatedBackwards()
{
    if(zoom_distance_ + zoom_step_ <= max_zoom_distance_)
    {
        zoom_distance_ += zoom_step_;
        Repaint();
        event_handler_->OnCellMapWindowZoomLevelChanged(int(zoom_distance_/zoom_step_)-1);
    }
}



void CellMapWindow::DoBeginSelectionBlockDrag(const POINT& point, bool control_held, bool shift_held)
{
    if(shift_held)
    {
        if(anchor_placed_)
        {
            ClearSelectedCells();
            int first_x = std::min(cursor_cell_.x_, anchor_cell_.x_);
            int last_x  = std::max(cursor_cell_.x_, anchor_cell_.x_);
            int first_z = std::min(cursor_cell_.z_, anchor_cell_.z_);
            int last_z  = std::max(cursor_cell_.z_, anchor_cell_.z_);
            for(int z = first_z; z <= last_z; z++)
            {
                for(int x = first_x; x <= last_x; x++)
                {
                    AddSelectedCell(CellIndex(x, z));
                }
            }
        }
        else
        {
            ClearSelectedCells();
            AddSelectedCell(cursor_cell_);

            anchor_cell_ = cursor_cell_;
            anchor_placed_ = true;
        }
    }
    else if(control_held)
    {
        IsCellSelected(cursor_cell_) ? RemoveSelectedCell(cursor_cell_) : AddSelectedCell(cursor_cell_);

        anchor_cell_ = cursor_cell_;
        anchor_placed_ = true;
    }
    else
    {
        ClearSelectedCells();
        AddSelectedCell(cursor_cell_);

        anchor_cell_ = cursor_cell_;
        anchor_placed_ = true;
    }
}

void CellMapWindow::DoSelectionBlockDrag(const POINT& point, bool control_held, bool shift_held)
{
    if(!control_held)
    {
        ClearSelectedCells();
    }
    int first_x = std::min(cursor_cell_.x_, anchor_cell_.x_);
    int last_x  = std::max(cursor_cell_.x_, anchor_cell_.x_);
    int first_z = std::min(cursor_cell_.z_, anchor_cell_.z_);
    int last_z  = std::max(cursor_cell_.z_, anchor_cell_.z_);
    for(int z = first_z; z <= last_z; z++)
    {
        for(int x = first_x; x <= last_x; x++)
        {
            AddSelectedCell(CellIndex(x, z));
        }
    }
}



Light* CellMapWindow::LineToLightIntersection(const POINT& point)
{
    Math::LineSegment ls = MouseCursorWorldLineSegment(point.x, point.y);

    // Make a tiny 6 plane BSP tree out of each light, and trace the line
    // segment through it.

    LightList& ll = map_doc_->GetLightList();
    LightList::iterator i;

    for(i = ll.begin(); i != ll.end(); ++i)
    {
        Bsp::Tree t;
        t.Insert(Math::Plane(Math::Vector(0.0f, 1.0f, 0.0f), i->Position() + Math::Vector(0.0f, 5.0f, 0.0f)), i->Position() + Math::Vector(2.0f, 5.0f, 0.0f));
        t.Insert(Math::Plane(Math::Vector(1.0f, 0.0f, 0.0f), i->Position() + Math::Vector(5.0f, 0.0f, 0.0f)), i->Position() + Math::Vector(5.0f, 2.0f, 0.0f));
        t.Insert(Math::Plane(Math::Vector(0.0f, 0.0f, 1.0f), i->Position() + Math::Vector(0.0f, 0.0f, 5.0f)), i->Position() + Math::Vector(0.0f, 2.0f, 5.0f));
        t.Insert(Math::Plane(Math::Vector(0.0f, -1.0f, 0.0f), i->Position() + Math::Vector(0.0f, -5.0f, 0.0f)), i->Position() + Math::Vector(2.0f, -5.0f, 0.0f));
        t.Insert(Math::Plane(Math::Vector(-1.0f, 0.0f, 0.0f), i->Position() + Math::Vector(-5.0f, 0.0f, 0.0f)), i->Position() + Math::Vector(-5.0f, 2.0f, 0.0f));
        t.Insert(Math::Plane(Math::Vector(0.0f, 0.0f, -1.0f), i->Position() + Math::Vector(0.0f, 0.0f, -5.0f)), i->Position() + Math::Vector(0.0f, 2.0f, -5.0f));

        if(!t.IsLineOfSight(ls, 0.0f))
        {
            return &(*i);
        }
    }

    return NULL;
}

void CellMapWindow::SelectLight(Light* l)
{
    SelectedLight sl;
    sl.light_ = l;

    LightSelectionList::iterator i = std::find(selected_lights_.begin(), selected_lights_.end(), l);
    if(i == selected_lights_.end())
    {
        selected_lights_.push_back(sl);
    }

    std::vector<std::string> light_names;
    LightSelectionList::iterator itor;
    for(itor = selected_lights_.begin(); itor != selected_lights_.end(); ++itor)
    {
        light_names.push_back(itor->light_->Name());
    }

    event_handler_->OnCellMapLightsSelected(light_names);
}

void CellMapWindow::DeselectLight(Light* l)
{
    LightSelectionList::iterator i = std::find(selected_lights_.begin(), selected_lights_.end(), l);
    if(i != selected_lights_.end())
    {
        selected_lights_.erase(i);
    }

    std::vector<std::string> light_names;
    LightSelectionList::iterator itor;
    for(itor = selected_lights_.begin(); itor != selected_lights_.end(); ++itor)
    {
        light_names.push_back(itor->light_->Name());
    }

    event_handler_->OnCellMapLightsSelected(light_names);
}

bool CellMapWindow::IsLightSelected(Light* l) const
{
    LightSelectionList::const_iterator i = std::find(selected_lights_.begin(), selected_lights_.end(), l);
    return i != selected_lights_.end();
}

void CellMapWindow::ClearSelectedLights()
{
    selected_lights_.clear();
    event_handler_->OnCellMapLightSelectNone();
}
