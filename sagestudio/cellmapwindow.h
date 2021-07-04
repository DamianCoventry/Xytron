#ifndef INCLUDED_CELLMAPWINDOW
#define INCLUDED_CELLMAPWINDOW

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>
#include <list>

#include "worldtypes.h"

#include "../gfx/gfx.h"
#include "../math/math.h"
#include "../util/resourcecontext.h"

class Light;

struct CellMapWindowEvents
{
    virtual void OnCellMapWindowZoomLevelChanged(int zoom) = 0;
    virtual void OnCellMapWindowInsertCell(const CellIndex& index, CellType type, float cell_size, const std::string& ts) = 0;
    virtual void OnCellMapWindowRemoveCell(const CellIndex& index) = 0;
    virtual void OnCellMapWindowSetTextureSet(const CellIndex& index, const std::string& ts) = 0;
    virtual void OnCellMapWindowSetSkyFlag(const CellIndex& index, bool sky) = 0;

    virtual void OnCellMapLightsSelected(const std::vector<std::string>& light_names) = 0;
    virtual void OnCellMapLightSelectNone() = 0;
    virtual void OnCellMapLightPositionChanged(const std::vector<Light*>& lights) = 0;
    virtual void OnCellMapLightRadiusChanged(const std::vector<Light*>& lights) = 0;
};

class CellMapDocument;
class Light;

class CellMapWindow
{
public:
    CellMapWindow(HINSTANCE instance, HWND parent, CellMapWindowEvents* event_handler, Aud::Device* audio_device);
    ~CellMapWindow();

    void RepositionWithinParent();
    void SetCellMapDocument(CellMapDocument* map_doc);

    Util::ResourceContext* GetResCxt() const { return (Util::ResourceContext*)&res_cxt_; }
    void UnloadResourceContext();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

    void ShowGrid(bool show);
    void SetViewStyle(ViewStyle vs);
    void SetEditMode(EditMode em)       { edit_mode_ = em; Repaint(); }
    void SetZoomLevel(int zoom);
    void SetCellBrush(CellBrush brush);
    CellBrush GetCellBrush() const      { return cell_brush_; }

    ViewStyle GetViewStyle() const  { return view_style_; }
    EditMode GetEditMode() const    { return edit_mode_; }

    void SetSelectedCellsCellType(CellType cell_type);
    void ClearSelectedCells()       { selected_cells_.clear(); Repaint(); }
    bool AreCellsSelected() const   { return !selected_cells_.empty(); }

    void SetTextureSetBrush(TextureSetBrush ts_brush);
    TextureSetBrush GetTextureSetBrush(TextureSetBrush ts_brush) const  { return ts_brush_; }
    void SetTextureSet(const std::string& texure_set)                   { texture_set_ = texure_set; }
    void SetDefaultTextureSet(const std::string& ts)                    { default_texture_set_ = ts; }

    void SetSelectedCellsTextureSet(const std::string& ts);

    void SetSkyBrush(SkyBrush sky_brush);
    void SetSelectedCellsToSky();
    void ClearSelectedCellsSky();
    void SkyTextureFilename(const std::string& content_dir, const std::string& filename);

    bool HaveCurrentLightCell() const       { return have_current_light_cell_; }
    CellIndex GetCurrentLightCell() const   { return current_light_cell_; }

    void SelectLights(const std::vector<std::string>& light_names);
    void SetCurrentLightTool(LightTool tool);
    void InsertLight(const std::string& name);
    void RemoveLights(const std::vector<std::string>& light_names);
    void SetLightRadius(const std::vector<std::string>& light_names, int radius);
    void CurrentLightCellSetUseWorldAmbient();
    void CurrentLightCellSetSpecifyAmbient();
    void CurrentLightCellSetAmbientSpecificLight(int ambient);
    void SetAmbientWorldLight(int ambient);
    void SetLightsColor(const std::vector<std::string>& light_names, DWORD color);
    void ViewLight(const std::string& light_name);

    void Repaint();

private:
    void Register();
    void Unregister();
    void Create(HWND parent);
    void Delete();

    void DrawGrid();
    void DrawMouseCursorCellFocus();
    void DrawMouseCursorXZIntersection();
    void DrawCells();
    void DrawSelectedCells();
    void DrawSkyCellsAsTextures();
    void DrawSkyCellsAsLines();
    void DrawLights();

    CellType ToCellType(CellBrush brush) const;

    void SetupModelViewTransform();
    Math::Vector WindowToWorldCoords(int client_x, int client_y);
    Math::LineSegment MouseCursorWorldLineSegment(int client_x, int client_y);

    void AddSelectedCell(const CellIndex& c);
    void RemoveSelectedCell(const CellIndex& c);
    bool IsCellSelected(const CellIndex& c) const;

    friend LRESULT CALLBACK CellMapWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnWindowPaint(HDC dc);

    void OnLmButtonDown(const POINT& point, bool control_held, bool shift_held);
    void OnLmButtonUp(const POINT& point, bool control_held, bool shift_held);
    void OnLmButtonDblClk(const POINT& point, bool control_held, bool shift_held);
    void OnRmButtonDown(const POINT& point, bool control_held, bool shift_held);
    void OnRmButtonUp(const POINT& point, bool control_held, bool shift_held);
    void OnRmButtonDblClk(const POINT& point, bool control_held, bool shift_held);
    void OnMouseMove(const POINT& point, bool control_held, bool shift_held);

    void OnMouseWheelRotatedForwards();
    void OnMouseWheelRotatedBackwards();

    void DoBeginSelectionBlockDrag(const POINT& point, bool control_held, bool shift_held);
    void DoSelectionBlockDrag(const POINT& point, bool control_held, bool shift_held);

    Light* LineToLightIntersection(const POINT& point);
    void SelectLight(Light* l);
    void DeselectLight(Light* l);
    bool IsLightSelected(Light* l) const;
    void ClearSelectedLights();

    void OnCalculatingLightingTotal(int total);
    void OnCalculatingLightingStep(int value);

private:
    static const char* window_title_;
    static const char* class_name_;

    CellMapWindowEvents* event_handler_;
    CellMapDocument* map_doc_;

    HINSTANCE instance_;
    HWND handle_;
    POINT prev_mouse_;

    Util::ResourceContext res_cxt_;

    Gfx::Graphics graphics_;
    Gfx::Viewport viewport_;
    Gfx::TexturePtr sky_texture_;

    CellIndex camera_cell_;
    CellIndex cursor_cell_;
    CellIndex anchor_cell_;
    ViewStyle view_style_;
    EditMode edit_mode_;
    CellBrush cell_brush_;
    TextureSetBrush ts_brush_;
    SkyBrush sky_brush_;
    LightTool current_light_tool_;

    bool first_paint_;
    bool anchor_placed_;
    bool show_grid_;
    bool dragging_camera_;
    bool dragging_cursor_;

    long max_x_cells_;
    long max_z_cells_;
    long window_width_;
    long window_height_;
    long draw_grid_cell_amount_;
    long draw_grid_cell_amount_half_;

    float cell_size_;
    float zoom_distance_;
    float zoom_x_rotation_;
    float zoom_step_;
    float max_zoom_distance_;

    Math::Vector camera_position_;
    Math::Vector camera_rotation_;
    Math::Vector cursor_intersection_;

    typedef std::list<CellIndex> CellSelectionList;
    CellSelectionList selected_cells_;

    std::string texture_set_;
    std::string default_texture_set_;

    bool have_current_light_cell_;
    CellIndex current_light_cell_;

    struct SelectedLight
    {
        bool operator==(Light* rhs) const
        {
            return light_ == rhs;
        }

        Light* light_;
        Math::Vector drag_offset_;
    };
    typedef std::list<SelectedLight> LightSelectionList;
    LightSelectionList selected_lights_;

    POINT prev_point_;
};

typedef boost::shared_ptr<CellMapWindow> CellMapWindowPtr;

#endif  // INCLUDED_CELLMAPWINDOW
