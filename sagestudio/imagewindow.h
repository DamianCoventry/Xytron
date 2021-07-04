#ifndef INCLUDED_IMAGEWINDOW
#define INCLUDED_IMAGEWINDOW

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <boost/shared_ptr.hpp>
#include <string>

namespace MyTest
{
class DibResources;
}

struct ImageWindowEvents
{
    virtual void OnImageModified() = 0;
    virtual void OnImageFrameMoved(const POINT& position) = 0;
    virtual void OnImageFrameResized(const SIZE& size) = 0;
};

class ImageWindow
{
public:
    ImageWindow(HINSTANCE instance, HWND parent, ImageWindowEvents* event_handler);
    ~ImageWindow();

    void RepositionWithinParent();

    enum MouseAction { MA_MOVEIMAGE, MA_RESIZEIMAGE, MA_MOVEVIEW };
    void SetMouseAction(MouseAction action) { mouse_action_ = action; }
    void DibResources(MyTest::DibResources* dib_resources)  { dib_resources_ = dib_resources; }

    void Zoom(int zoom);
    void EnableGridSnap(bool enable);
    void GridSnap(int grid_snap);

    void Name(const std::string& name);
    void Texture(const std::string& filename);
    void Position(const POINT& position);
    void Size(const SIZE& size);
    void UseFrameCount(bool use) { use_frame_count_ = use; }
    void FrameCount(int frame_count);

    void Clear();
    void Enable();
    void Disable();
    void Show();
    void Hide();

private:
    void Register();
    void Unregister();
    void Create(HWND parent);
    void Delete();

    void CreateDoubleBuffer();
    void DeleteDoubleBuffer();

    HBITMAP LoadDib(HDC dc, const std::string& filename, int& dib_width, int& dib_height) const;

    void UpdateThumbnailInCache();

    friend LRESULT CALLBACK ImageWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnWindowPaint(HDC dc);
    void OnLmButtonDown(const POINT& point);
    void OnLmButtonUp(const POINT& point);
    void OnMouseMove(const POINT& point);

private:
    static const char* window_title_;
    static const char* class_name_;

    std::string name_;
    std::string texture_filename_;

    ImageWindowEvents* event_handler_;
    MouseAction mouse_action_;
    MyTest::DibResources* dib_resources_;

    HINSTANCE instance_;
    HWND handle_;
    HDC double_buffer_dc_;
    HBITMAP double_buffer_bitmap_;
    HBITMAP texture_handle_;
    HPEN single_frame_pen_;
    HPEN multiple_frame_pen_;

    bool use_frame_count_;
    bool dragging_;
    bool resizing_;
    bool moving_view_;
    bool grid_snap_enabled_;

    POINT drag_offset_;
    POINT prev_pos_;
    POINT scroll_offset_;
    POINT position_;

    SIZE size_;
    SIZE zoom_size_;
    SIZE grid_snap_resize_;

    int zoom_;
    int grid_snap_;
    int texture_width_;
    int texture_height_;
    int frame_count_;
};

typedef boost::shared_ptr<ImageWindow> ImageWindowPtr;

#endif  // INCLUDED_IMAGEWINDOW
