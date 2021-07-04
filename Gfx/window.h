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
#ifndef INCLUDED_GFX_WINDOW
#define INCLUDED_GFX_WINDOW

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace Gfx
{

class Window;

///////////////////////////////////////////////////////////////////////////////
struct IWindowEvents
{
    virtual void OnWindowCreated(Window* window) {}
    virtual void OnWindowDestroyed(Window* window) {}
    virtual bool OnWindowClosed(Window* window) { return true; }                  // true = yes, close the window
    virtual void OnWindowKeyPressed(Window* window, int vkey) {}
    virtual void OnWindowKeyReleased(Window* window, int vkey) {}
    virtual void OnWindowMouseMoved(Window* window, int mouse_x, int mouse_y) {}
    virtual void OnWindowMouseLPressed(Window* window) {}
    virtual void OnWindowMouseLReleased(Window* window) {}
    virtual void OnWindowMouseLDoubleClicked(Window* window) {}
    virtual void OnWindowMouseRPressed(Window* window) {}
    virtual void OnWindowMouseRReleased(Window* window) {}
    virtual void OnWindowMouseRDoubleClicked(Window* window) {}
    virtual void OnWindowActivated(Window* window) {}
    virtual void OnWindowDeactived(Window* window) {}
    virtual void OnWindowPaint(Window* window, HDC dc) {}
    virtual bool OnWindowEraseBackground(Window* window, HDC dc) { return true; }   // true = yes, erase the background
    virtual void OnWindowOtherMessage(UINT msg, WPARAM wparam, LPARAM lparam) {}
    virtual void OnMouseWheelRotatedBackwards(short delta) {}
    virtual void OnMouseWheelRotatedForwards(short delta) {}
};

///////////////////////////////////////////////////////////////////////////////
class Window
{
public:
    static void Register(HINSTANCE instance, int icon_id = 0);
    static void Unregister(HINSTANCE instance);

public:
    Window(HINSTANCE instance)
        : instance_(instance), handle_(NULL), event_handler_(NULL) {}
    ~Window() { Delete(); }

    bool CreateFullscreen(int width, int height, const std::string& window_title, IWindowEvents* event_handler);
    bool CreateWindowed(int width, int height, const std::string& window_title, IWindowEvents* event_handler);
    void Delete();

    void Show();
    void Hide();

    void SetTitle(const std::string& title);

    void SetFullscreenStyle();
    void SetWindowedStyle();
    void ToggleBetweenStyles();
    bool IsFullscreenStyle() const;

    HINSTANCE Instance() const  { return instance_; }
    HWND Handle() const         { return handle_; }

private:
    static LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    static const char* class_name_;
    static DWORD style_windowed_;
    static DWORD style_fullscreen_;
    static DWORD style_ex_windowed_;
    static DWORD style_ex_fullscreen_;

    HINSTANCE instance_;
    HWND handle_;
    IWindowEvents* event_handler_;
};

}       // namespace Gfx

#endif  // INCLUDED_GFX_WINDOW
