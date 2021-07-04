#include "templatewindow.h"
#include "resource.h"
#include <stdexcept>
#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'TemplateWindow *' of greater size

const char* TemplateWindow::window_title_ = "TemplateWindow";
const char* TemplateWindow::class_name_   = "TemplateWindow";

TemplateWindow::TemplateWindow(HINSTANCE instance, HWND parent, TemplateWindowEvents* event_handler)
: instance_(instance)
, handle_(NULL)
, event_handler_(event_handler)
{
    Register();
    Create(parent);
}

TemplateWindow::~TemplateWindow()
{
    Delete();
    Unregister();
}





void TemplateWindow::Register()
{
    WNDCLASS wc;
    wc.style            = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc      = TemplateWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = instance_;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = class_name_;

    if(!RegisterClass(&wc))
    {
        throw std::runtime_error("Unable to register the TemplateWindow's window class");
    }
}

void TemplateWindow::Unregister()
{
    UnregisterClass(class_name_, instance_);
}

void TemplateWindow::Create(HWND parent)
{
    handle_ = CreateWindowEx(WS_EX_CLIENTEDGE, class_name_, window_title_, WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, NULL, instance_,
        reinterpret_cast<void*>(this));

    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to register TemplateWindow");
    }
}

void TemplateWindow::Delete()
{
    if(handle_)
    {
        DestroyWindow(handle_);
        handle_ = NULL;
    }
}

void TemplateWindow::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, 200, 0, parent_client_rect.right-200, parent_client_rect.bottom, SWP_NOZORDER);
}

void TemplateWindow::Enable()
{
    EnableWindow(handle_, TRUE);
}

void TemplateWindow::Disable()
{
    EnableWindow(handle_, FALSE);
}

void TemplateWindow::Show()
{
    ShowWindow(handle_, SW_SHOW);
}

void TemplateWindow::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void TemplateWindow::Clear()
{
}



LRESULT CALLBACK TemplateWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(window, GWL_USERDATA);
    TemplateWindow* this_ = reinterpret_cast<TemplateWindow*>(data);

    switch(msg)
    {
    case WM_CREATE:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(window, GWL_USERDATA, reinterpret_cast<LONG>(create_struct->lpCreateParams));
            this_ = reinterpret_cast<TemplateWindow*>(create_struct->lpCreateParams);
            break;
        }
    }

    return DefWindowProc(window, msg, wparam, lparam);
}
