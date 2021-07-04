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
#include "stdafx.h"
#include "window.h"
#include <zmouse.h>
#include "../util/throw.h"

#pragma warning(disable : 4311)         // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)         // 'reinterpret_cast' : conversion from 'LONG' to 'Gfx::Window *' of greater size

using namespace Gfx;

const char* Window::class_name_ = "GfxLibWindowClass";

DWORD Window::style_windowed_       = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
DWORD Window::style_fullscreen_     = WS_POPUP;
DWORD Window::style_ex_windowed_    = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
DWORD Window::style_ex_fullscreen_  = WS_EX_TOPMOST;




///////////////////////////////////////////////////////////////////////////////
void Window::Register(HINSTANCE instance, int icon_id)
{
    WNDCLASS wc;
    wc.style            = CS_DBLCLKS | CS_OWNDC;
    wc.lpfnWndProc      = WindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = instance;
    wc.hIcon            = (icon_id ? LoadIcon(instance, MAKEINTRESOURCE(icon_id)) : LoadIcon(NULL, IDI_WINLOGO));
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = class_name_;

    if(!RegisterClass(&wc))
    {
        THROW_WIN32("Couldn't register the main window's window class");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Window::Unregister(HINSTANCE instance)
{
    UnregisterClass(class_name_, instance);
}




///////////////////////////////////////////////////////////////////////////////
bool Window::CreateFullscreen(int width, int height, const std::string& window_title, IWindowEvents* event_handler)
{
    LOG("Creating a fullscreen window [" << width << "x" << height << "], [" << window_title << "]");

    event_handler_ = event_handler;
    handle_ = CreateWindowEx(style_ex_fullscreen_, class_name_, window_title.c_str(),
        style_fullscreen_, 0, 0, width, height, GetDesktopWindow(), NULL, instance_,
        reinterpret_cast<void*>(this));
    return handle_ != NULL;
}

///////////////////////////////////////////////////////////////////////////////
bool Window::CreateWindowed(int width, int height, const std::string& window_title, IWindowEvents* event_handler)
{
    LOG("Creating a windowed window [" << width << "x" << height << "], [" << window_title << "]");

    event_handler_ = event_handler;
    RECT rect;
    rect.left = rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRectEx(&rect, style_windowed_, FALSE, style_ex_windowed_);

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    int x = (GetSystemMetrics(SM_CXSCREEN)/2) - (w/2);
    int y = (GetSystemMetrics(SM_CYSCREEN)/2) - (h/2);

    handle_ = CreateWindowEx(style_ex_windowed_, class_name_, window_title.c_str(),
        style_windowed_, x, y, w, h, GetDesktopWindow(), NULL, instance_,
        reinterpret_cast<void*>(this));
    return handle_ != NULL;
}

///////////////////////////////////////////////////////////////////////////////
void Window::Delete()
{
    if(handle_)
    {
        LOG("Deleting the main window's handle");
        DestroyWindow(handle_);
        handle_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Window::Show()
{
    LOG("Showing the main window");

    ShowWindow(handle_, SW_SHOWNORMAL);
    UpdateWindow(handle_);
    SetForegroundWindow(handle_);
}

///////////////////////////////////////////////////////////////////////////////
void Window::Hide()
{
    LOG("Hiding the main window");

    ShowWindow(handle_, SW_HIDE);
}

///////////////////////////////////////////////////////////////////////////////
void Window::SetTitle(const std::string& title)
{
    SetWindowText(handle_, title.c_str());
}

///////////////////////////////////////////////////////////////////////////////
void Window::SetFullscreenStyle()
{
    LOG("Setting the main window to fullscreen style");

    RECT rect;
    GetClientRect(handle_, &rect);

    ShowWindow(handle_, SW_HIDE);

    SetWindowLongPtr(handle_, GWL_STYLE, style_fullscreen_);
    SetWindowLongPtr(handle_, GWL_EXSTYLE, style_ex_fullscreen_);

    SetWindowPos(handle_, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    SetWindowPos(handle_, NULL, 0, 0, rect.right, rect.bottom, SWP_NOZORDER);

    ShowWindow(handle_, SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////
void Window::SetWindowedStyle()
{
    LOG("Setting the main window to windowed style");

    RECT rect;
    GetClientRect(handle_, &rect);

    ShowWindow(handle_, SW_HIDE);

    SetWindowLongPtr(handle_, GWL_STYLE, style_windowed_);
    SetWindowLongPtr(handle_, GWL_EXSTYLE, style_ex_windowed_);

    SetWindowPos(handle_, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    AdjustWindowRectEx(&rect, style_windowed_, FALSE, style_ex_windowed_);

    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    int x = (GetSystemMetrics(SM_CXSCREEN)/2) - (w/2);
    int y = (GetSystemMetrics(SM_CYSCREEN)/2) - (h/2);

    SetWindowPos(handle_, NULL, x, y, w, h, SWP_NOZORDER);

    ShowWindow(handle_, SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////
void Window::ToggleBetweenStyles()
{
    bool fullscreen = ((GetWindowLongPtr(handle_, GWL_STYLE) & WS_POPUP) != 0);
    fullscreen ? SetWindowedStyle() : SetFullscreenStyle();
}

///////////////////////////////////////////////////////////////////////////////
bool Window::IsFullscreenStyle() const
{
    return ((GetWindowLongPtr(handle_, GWL_STYLE) & WS_POPUP) != 0);
}




///////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK Window::WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG_PTR data = GetWindowLongPtr(window, GWLP_USERDATA);
    Window* this_ = reinterpret_cast<Window*>(data);

    if(msg != WM_CREATE && this_ == NULL)
    {
        return DefWindowProc(window, msg, wparam, lparam);
    }

    switch(msg)
    {
    case WM_ERASEBKGND:
        if(this_->event_handler_->OnWindowEraseBackground(this_, (HDC)wparam))
        {
            break;
        }
        return 1;       // Don't let the system erase the background
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(window, &ps);
            this_->event_handler_->OnWindowPaint(this_, dc);
            EndPaint(window, &ps);
            break;
        }
    case WM_MOUSEWHEEL:
        {
            short delta = short(HIWORD(wparam));
            if(delta < 0)
            {
                this_->event_handler_->OnMouseWheelRotatedBackwards(delta);
            }
            else
            {
                this_->event_handler_->OnMouseWheelRotatedForwards(delta);
            }
            return 0;
        }
    case WM_CREATE:
        {
            CREATESTRUCT* info = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(info->lpCreateParams));
            this_ = reinterpret_cast<Window*>(info->lpCreateParams);
            this_->event_handler_->OnWindowCreated(this_);
            break;
        }
    case WM_DESTROY:
        this_->event_handler_->OnWindowDestroyed(this_);
        break;
    case WM_CLOSE:
        if(this_->event_handler_->OnWindowClosed(this_))
        {
            break;
        }
        return 0;       // Don't let the system close the window
    case WM_KEYDOWN:
        this_->event_handler_->OnWindowKeyPressed(this_, int(wparam));
        break;
    case WM_KEYUP:
        this_->event_handler_->OnWindowKeyReleased(this_, int(wparam));
        break;
    case WM_LBUTTONDOWN:
        this_->event_handler_->OnWindowMouseLPressed(this_);
        break;
    case WM_LBUTTONUP:
        this_->event_handler_->OnWindowMouseLReleased(this_);
        break;
    case WM_LBUTTONDBLCLK:
        this_->event_handler_->OnWindowMouseLDoubleClicked(this_);
        break;
    case WM_RBUTTONDOWN:
        this_->event_handler_->OnWindowMouseRPressed(this_);
        break;
    case WM_RBUTTONUP:
        this_->event_handler_->OnWindowMouseRReleased(this_);
        break;
    case WM_RBUTTONDBLCLK:
        this_->event_handler_->OnWindowMouseRDoubleClicked(this_);
        break;
    case WM_MOUSEMOVE:
        this_->event_handler_->OnWindowMouseMoved(this_, LOWORD(lparam), HIWORD(lparam));
        break;
    case WM_ACTIVATE:
        LOWORD(wparam) == WA_INACTIVE ?
            this_->event_handler_->OnWindowDeactived(this_) : this_->event_handler_->OnWindowActivated(this_);
        break;
    default:
        if(this_)
        {
            this_->event_handler_->OnWindowOtherMessage(msg, wparam, lparam);
        }
        break;
    }

    return DefWindowProc(window, msg, wparam, lparam);
}
