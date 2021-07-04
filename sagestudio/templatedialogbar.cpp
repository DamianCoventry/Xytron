#include "templatedialogbar.h"
#include "resource.h"
#include <stdexcept>
#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'TemplateDialogBar *' of greater size

TemplateDialogBar::TemplateDialogBar(HINSTANCE instance, HWND parent, TemplateDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_TEMPLATE_DIALOGBAR), parent,
        TemplateDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an TemplateDialogBar instance");
    }
}

TemplateDialogBar::~TemplateDialogBar()
{
    DestroyWindow(handle_);
}

void TemplateDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, 0, 0, 300, parent_client_rect.bottom, SWP_NOZORDER);
}

void TemplateDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void TemplateDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void TemplateDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
}

void TemplateDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void TemplateDialogBar::Clear()
{
}





INT_PTR CALLBACK TemplateDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    TemplateDialogBar* this_ = reinterpret_cast<TemplateDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<TemplateDialogBar*>(lparam);
            this_->handle_ = dialog;
            this_->OnInitDialog();
            return TRUE;        // Let the system call SetFocus()
        }
    }

    return FALSE;
}

void TemplateDialogBar::OnInitDialog()
{
}
