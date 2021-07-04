#include "imagewindow.h"
#include "resource.h"
#include "dibresources.h"

#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <commctrl.h>
#include <windowsx.h>

#include "../gfx/gfx.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ImageWindow *' of greater size

const char* ImageWindow::window_title_ = "ImageWindow";
const char* ImageWindow::class_name_   = "ImageWindow";

ImageWindow::ImageWindow(HINSTANCE instance, HWND parent, ImageWindowEvents* event_handler)
: instance_(instance)
, handle_(NULL)
, event_handler_(event_handler)
, double_buffer_dc_(NULL)
, double_buffer_bitmap_(NULL)
, dib_resources_(NULL)
, texture_handle_(0)
, texture_width_(0)
, texture_height_(0)
, frame_count_(0)
, single_frame_pen_(NULL)
, multiple_frame_pen_(NULL)
, use_frame_count_(false)
, dragging_(false)
, resizing_(false)
, moving_view_(false)
, mouse_action_(MA_MOVEIMAGE)
, zoom_(100)
, grid_snap_enabled_(true)
, grid_snap_(10)
{
    memset(&position_, 0, sizeof(POINT));
    memset(&size_, 0, sizeof(SIZE));

    scroll_offset_.x = scroll_offset_.y = 0;
    zoom_size_.cx = zoom_size_.cy = 0;
    grid_snap_resize_.cx = grid_snap_resize_.cy = 0;

    single_frame_pen_   = CreatePen(PS_DASH, 0, RGB(255, 127, 0));
    multiple_frame_pen_ = CreatePen(PS_DOT, 0, RGB(127, 127, 127));

    Register();
    Create(parent);
}

ImageWindow::~ImageWindow()
{
    DeletePen(multiple_frame_pen_);
    DeletePen(single_frame_pen_);

    Delete();
    Unregister();
}

void ImageWindow::Zoom(int zoom)
{
    zoom_ = zoom;

    float scale = float(zoom) / 100.0f;
    zoom_size_.cx = long(float(texture_width_) * scale);
    zoom_size_.cy = long(float(texture_height_) * scale);

    InvalidateRect(handle_, NULL, FALSE);
}

void ImageWindow::EnableGridSnap(bool enable)
{
    grid_snap_enabled_ = enable;
}

void ImageWindow::GridSnap(int grid_snap)
{
    grid_snap_ = grid_snap;
}

void ImageWindow::Name(const std::string& name)
{
    name_ = name;
}

void ImageWindow::Texture(const std::string& filename)
{
    if(texture_handle_)
    {
        DeleteObject(texture_handle_);
    }

    texture_filename_ = filename;

    HDC dc = GetDC(handle_);
    texture_handle_ = LoadDib(dc, texture_filename_, texture_width_, texture_height_);
    ReleaseDC(handle_, dc);

    float scale = float(zoom_) / 100.0f;
    zoom_size_.cx = long(float(texture_width_) * scale);
    zoom_size_.cy = long(float(texture_height_) * scale);

    UpdateThumbnailInCache();
    InvalidateRect(handle_, NULL, FALSE);
}

void ImageWindow::Position(const POINT& position)
{
    position_ = position;
    UpdateThumbnailInCache();
    InvalidateRect(handle_, NULL, FALSE);
}

void ImageWindow::Size(const SIZE& size)
{
    size_ = size;
    UpdateThumbnailInCache();
    InvalidateRect(handle_, NULL, FALSE);
}

void ImageWindow::FrameCount(int frame_count)
{
    frame_count_ = frame_count;
    InvalidateRect(handle_, NULL, FALSE);
}

void ImageWindow::Register()
{
    WNDCLASS wc;
    wc.style            = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc      = ImageWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = instance_;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_CROSS);
    wc.hbrBackground    = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = class_name_;

    if(!RegisterClass(&wc))
    {
        throw std::runtime_error("Unable to register the ImageWindow's window class");
    }
}

void ImageWindow::Unregister()
{
    UnregisterClass(class_name_, instance_);
}

void ImageWindow::Create(HWND parent)
{
    handle_ = CreateWindowEx(WS_EX_CLIENTEDGE, class_name_, window_title_, WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, NULL, instance_,
        reinterpret_cast<void*>(this));

    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to register ImageWindow");
    }

    CreateDoubleBuffer();
}

void ImageWindow::Delete()
{
    if(handle_)
    {
        DestroyWindow(handle_);
        handle_ = NULL;
    }
}

void ImageWindow::CreateDoubleBuffer()
{
    RECT rect;
    GetClientRect(handle_, &rect);

    HDC dc                  = GetDC(handle_);
    double_buffer_dc_       = CreateCompatibleDC(dc);
    double_buffer_bitmap_   = CreateCompatibleBitmap(dc, rect.right, rect.bottom);

    SelectObject(double_buffer_dc_, double_buffer_bitmap_);
    ReleaseDC(handle_, dc);
}

void ImageWindow::DeleteDoubleBuffer()
{
    DeleteBitmap(double_buffer_bitmap_);
    DeleteDC(double_buffer_dc_);
}

void ImageWindow::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, 200, 40, parent_client_rect.right-500, parent_client_rect.bottom-40, SWP_NOZORDER);

    DeleteDoubleBuffer();
    CreateDoubleBuffer();
}

void ImageWindow::Enable()
{
    EnableWindow(handle_, TRUE);
}

void ImageWindow::Disable()
{
    EnableWindow(handle_, FALSE);
}

void ImageWindow::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void ImageWindow::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void ImageWindow::Clear()
{
}

HBITMAP ImageWindow::LoadDib(HDC dc, const std::string& filename, int& dib_width, int& dib_height) const
{
    int bpp;
    boost::shared_array<unsigned char> pixels;

    if(boost::algorithm::ifind_first(filename, ".bmp"))
    {
        Gfx::BitmapFile file;
        file.Load(filename);
        dib_width   = file.GetWidth();
        dib_height  = file.GetHeight();
        bpp     = file.GetBpp();
        pixels  = file.Pixels();
    }
    else if(boost::algorithm::ifind_first(filename, ".tga"))
    {
        Gfx::TargaFile file;
        file.Load(filename, false, false);
        dib_width   = file.GetWidth();
        dib_height  = file.GetHeight();
        bpp     = file.GetBpp();
        pixels  = file.Pixels();
    }
    else if(boost::algorithm::ifind_first(filename, ".png"))
    {
        Gfx::PngFile file;
        file.Load(filename, true, false);
        dib_width   = file.GetWidth();
        dib_height  = file.GetHeight();
        bpp     = file.GetBpp();
        pixels  = file.Pixels();
    }
    else
    {
        return NULL;
    }

    // Create a bitmap the same size as the entire image.
    BITMAPINFO bi;
    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize             = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth            = dib_width;
    bi.bmiHeader.biHeight           = dib_height;
    bi.bmiHeader.biPlanes           = 1;
    bi.bmiHeader.biBitCount         = bpp;
    bi.bmiHeader.biCompression      = BI_RGB;

    unsigned char* dest_bits;
    HBITMAP bitmap_handle = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)&dest_bits, NULL, 0);
    if(bitmap_handle == NULL)
    {
        return NULL;
    }

    // Copy all the pixels from the file into the bitmap
    unsigned long scanline_length = (bpp >> 3) * dib_width;
    unsigned char* source_bits = pixels.get();
    for(int y = 0; y < dib_height; y++)
    {
        memcpy(dest_bits, source_bits, scanline_length);
        source_bits += scanline_length;
        dest_bits += scanline_length;
    }

    return bitmap_handle;
}

void ImageWindow::UpdateThumbnailInCache()
{
    std::string unique_name(use_frame_count_ ? "ImageAnim:" : "Image:");
    unique_name += name_;

    HDC dc = GetDC(handle_);
    dib_resources_->Create(dc, texture_filename_, position_, size_, unique_name);
    ReleaseDC(handle_, dc);
}




LRESULT CALLBACK ImageWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(window, GWL_USERDATA);
    ImageWindow* this_ = reinterpret_cast<ImageWindow*>(data);

    switch(msg)
    {
    case WM_CREATE:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(window, GWL_USERDATA, reinterpret_cast<LONG>(create_struct->lpCreateParams));
            this_ = reinterpret_cast<ImageWindow*>(create_struct->lpCreateParams);
            break;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(window, &ps);
            this_->OnWindowPaint(dc);
            EndPaint(window, &ps);
            break;
        }
    case WM_LBUTTONDOWN:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnLmButtonDown(point);
            break;
        }
    case WM_LBUTTONUP:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnLmButtonUp(point);
            break;
        }
    case WM_MOUSEMOVE:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnMouseMove(point);
            break;
        }
    }

    return DefWindowProc(window, msg, wparam, lparam);
}





void ImageWindow::OnWindowPaint(HDC dc)
{
    RECT rect;
    GetClientRect(handle_, &rect);

    FillRect(double_buffer_dc_, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    HDC mem_dc = CreateCompatibleDC(dc);
    HBITMAP old_bitmap = SelectBitmap(mem_dc, texture_handle_);

    StretchBlt(
        double_buffer_dc_,
        scroll_offset_.x, scroll_offset_.y,
        zoom_size_.cx, zoom_size_.cy,
        mem_dc,
        0, 0,
        texture_width_, texture_height_,
        SRCCOPY);

    SelectBitmap(mem_dc, old_bitmap);
    DeleteDC(mem_dc);

    float scale = float(zoom_) / 100.0f;
    SIZE scaled_size;
    scaled_size.cx = long(float(size_.cx) * scale);
    scaled_size.cy = long(float(size_.cy) * scale);
    POINT scaled_pos;
    scaled_pos.x = long(float(position_.x) * scale);
    scaled_pos.y = long(float(position_.y) * scale);

    scaled_pos.x += scroll_offset_.x;
    scaled_pos.y += scroll_offset_.y;

    static const int point_count = 5;
    static POINT points[5];

    if(use_frame_count_)
    {
        HPEN old_pen = SelectPen(double_buffer_dc_, multiple_frame_pen_);

        POINT point;
        point = scaled_pos;
        for(int i = 0; i < frame_count_; i++)
        {
            if(point.x + scaled_size.cx > zoom_size_.cx)
            {
                point.x = scaled_pos.x;
                if(point.y + scaled_size.cy > zoom_size_.cy)
                {
                    // This amount of frames does not fit on this image
                    break;
                }
                point.y += scaled_size.cy;
            }

            points[0] = point;
            points[1].x = point.x + scaled_size.cx;
            points[1].y = point.y;
            points[2].x = point.x + scaled_size.cx;
            points[2].y = point.y + scaled_size.cy;
            points[3].x = point.x;
            points[3].y = point.y + scaled_size.cy;
            points[4] = points[0];
            Polyline(double_buffer_dc_, points, point_count);

            point.x += scaled_size.cx;
        }

        SelectPen(double_buffer_dc_, old_pen);
    }

    points[0] = scaled_pos;
    points[1].x = scaled_pos.x + scaled_size.cx;
    points[1].y = scaled_pos.y;
    points[2].x = scaled_pos.x + scaled_size.cx;
    points[2].y = scaled_pos.y + scaled_size.cy;
    points[3].x = scaled_pos.x;
    points[3].y = scaled_pos.y + scaled_size.cy;
    points[4] = points[0];

    HPEN old_pen = SelectPen(double_buffer_dc_, single_frame_pen_);
    Polyline(double_buffer_dc_, points, point_count);
    SelectPen(double_buffer_dc_, old_pen);

    BitBlt(dc, 0, 0, rect.right, rect.bottom, double_buffer_dc_, 0, 0, SRCCOPY);
}

void ImageWindow::OnLmButtonDown(const POINT& point)
{
    SetCapture(handle_);

    float scale = float(zoom_) / 100.0f;
    POINT scaled_point;
    scaled_point.x = long(float(point.x) / scale);
    scaled_point.y = long(float(point.y) / scale);

    scaled_point.x -= scroll_offset_.x;
    scaled_point.y -= scroll_offset_.y;

    if(scaled_point.x >= position_.x && scaled_point.x < (position_.x + size_.cx) &&
       scaled_point.y >= position_.y && scaled_point.y < (position_.y + size_.cy))
    {
        switch(mouse_action_)
        {
        case MA_MOVEIMAGE:
            drag_offset_.x = scaled_point.x - position_.x;
            drag_offset_.y = scaled_point.y - position_.y;
            dragging_ = true;
            break;
        case MA_RESIZEIMAGE:
            while(ShowCursor(FALSE) >= 0) ;
            prev_pos_ = scaled_point;
            resizing_ = true;
            break;
        case MA_MOVEVIEW:
            break;
        }
    }
    else if(mouse_action_ == MA_MOVEVIEW)
    {
        moving_view_ = true;
        prev_pos_ = point;
    }
}

void ImageWindow::OnLmButtonUp(const POINT& point)
{
    if(GetCapture() == handle_)
    {
        if(dragging_)
        {
            UpdateThumbnailInCache();
            InvalidateRect(handle_, NULL, FALSE);
            dragging_ = false;
        }
        if(resizing_)
        {
            while(ShowCursor(TRUE) < 0) ;
            UpdateThumbnailInCache();
            InvalidateRect(handle_, NULL, FALSE);
            resizing_ = false;
        }
        if(moving_view_)
        {
            InvalidateRect(handle_, NULL, FALSE);
            moving_view_ = false;
        }
        grid_snap_resize_.cx = grid_snap_resize_.cy = 0;
        ReleaseCapture();
    }
}

void ImageWindow::OnMouseMove(const POINT& point)
{
    if(GetCapture() == handle_)
    {
        float scale = float(zoom_) / 100.0f;
        POINT scaled_point;
        scaled_point.x = long(float(point.x) / scale);
        scaled_point.y = long(float(point.y) / scale);

        scaled_point.x -= scroll_offset_.x;
        scaled_point.y -= scroll_offset_.y;

        if(dragging_)
        {
            position_.x = scaled_point.x - drag_offset_.x;
            position_.y = scaled_point.y - drag_offset_.y;
            if(grid_snap_enabled_)
            {
                position_.x = (position_.x/grid_snap_) * grid_snap_;
                position_.y = (position_.y/grid_snap_) * grid_snap_;
            }
            event_handler_->OnImageModified();
            event_handler_->OnImageFrameMoved(position_);
            InvalidateRect(handle_, NULL, FALSE);
        }
        if(resizing_)
        {
            if(grid_snap_enabled_)
            {
                grid_snap_resize_.cx += scaled_point.x - prev_pos_.x;
                grid_snap_resize_.cy += scaled_point.y - prev_pos_.y;

                long x_delta = (grid_snap_resize_.cx/grid_snap_) * grid_snap_;
                long y_delta = (grid_snap_resize_.cy/grid_snap_) * grid_snap_;

                if(x_delta != 0)
                {
                    size_.cx += x_delta;
                    grid_snap_resize_.cx = 0;
                }

                if(y_delta != 0)
                {
                    size_.cy += y_delta;
                    grid_snap_resize_.cy = 0;
                }
            }
            else
            {
                size_.cx += scaled_point.x - prev_pos_.x;
                size_.cy += scaled_point.y - prev_pos_.y;
            }
            prev_pos_ = scaled_point;
            event_handler_->OnImageModified();
            event_handler_->OnImageFrameResized(size_);
            InvalidateRect(handle_, NULL, FALSE);
        }
        if(moving_view_)
        {
            if(grid_snap_enabled_)
            {
                grid_snap_resize_.cx += point.x - prev_pos_.x;
                grid_snap_resize_.cy += point.y - prev_pos_.y;

                long x_delta = (grid_snap_resize_.cx/grid_snap_) * grid_snap_;
                long y_delta = (grid_snap_resize_.cy/grid_snap_) * grid_snap_;

                if(x_delta != 0)
                {
                    scroll_offset_.x += x_delta;
                    grid_snap_resize_.cx = 0;
                }

                if(y_delta != 0)
                {
                    scroll_offset_.y += y_delta;
                    grid_snap_resize_.cy = 0;
                }
            }
            else
            {
                scroll_offset_.x += point.x - prev_pos_.x;
                scroll_offset_.y += point.y - prev_pos_.y;
            }
            prev_pos_ = point;
            InvalidateRect(handle_, NULL, FALSE);
        }
    }
}
