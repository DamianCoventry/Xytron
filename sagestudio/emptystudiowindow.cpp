#include "emptystudiowindow.h"
#include "resource.h"
#include <stdexcept>
#include <commctrl.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'EmptyStudioWindow *' of greater size

const char* EmptyStudioWindow::window_title_ = "EmptyStudioWindow";
const char* EmptyStudioWindow::class_name_   = "EmptyStudioWindow";

EmptyStudioWindow::EmptyStudioWindow(HINSTANCE instance, HWND parent)
: instance_(instance)
, handle_(NULL)
{
    Register();
    Create(parent);
}

EmptyStudioWindow::~EmptyStudioWindow()
{
    Delete();
    Unregister();
}





void EmptyStudioWindow::Register()
{
    WNDCLASS wc;
    wc.style            = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc      = EmptyStudioWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = instance_;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = class_name_;

    if(!RegisterClass(&wc))
    {
        throw std::runtime_error("Unable to register the EmptyStudioWindow's window class");
    }
}

void EmptyStudioWindow::Unregister()
{
    UnregisterClass(class_name_, instance_);
}

void EmptyStudioWindow::Create(HWND parent)
{
    handle_ = CreateWindowEx(WS_EX_CLIENTEDGE, class_name_, window_title_, WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, NULL, instance_,
        reinterpret_cast<void*>(this));

    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to register EmptyStudioWindow");
    }
}

void EmptyStudioWindow::Delete()
{
    if(handle_)
    {
        DestroyWindow(handle_);
        handle_ = NULL;
    }
}

void EmptyStudioWindow::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, 200, 0, parent_client_rect.right-200, parent_client_rect.bottom, SWP_NOZORDER);
}

void EmptyStudioWindow::Enable()
{
    EnableWindow(handle_, TRUE);
}

void EmptyStudioWindow::Disable()
{
    EnableWindow(handle_, FALSE);
}

void EmptyStudioWindow::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void EmptyStudioWindow::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}





LRESULT CALLBACK EmptyStudioWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(window, GWL_USERDATA);
    EmptyStudioWindow* this_ = reinterpret_cast<EmptyStudioWindow*>(data);

    switch(msg)
    {
    case WM_CREATE:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(window, GWL_USERDATA, reinterpret_cast<LONG>(create_struct->lpCreateParams));
            this_ = reinterpret_cast<EmptyStudioWindow*>(create_struct->lpCreateParams);
            break;
        }
    }

    return DefWindowProc(window, msg, wparam, lparam);
}
