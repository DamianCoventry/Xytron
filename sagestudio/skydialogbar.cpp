#include "skydialogbar.h"
#include "resource.h"
#include <stdexcept>
#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'SkyDialogBar *' of greater size

SkyDialogBar::SkyDialogBar(HINSTANCE instance, HWND parent, SkyDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_SKY_DIALOGBAR), parent,
        SkyDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an SkyDialogBar instance");
    }
}

SkyDialogBar::~SkyDialogBar()
{
    DestroyWindow(handle_);
}

void SkyDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, parent_client_rect.right-200, 0, 200, parent_client_rect.bottom, SWP_NOZORDER);
}

void SkyDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void SkyDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void SkyDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
}

void SkyDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void SkyDialogBar::Clear()
{
}





INT_PTR CALLBACK SkyDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    SkyDialogBar* this_ = reinterpret_cast<SkyDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<SkyDialogBar*>(lparam);
            this_->handle_ = dialog;
            this_->OnInitDialog();
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_CURRENTBRUSH:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxCurrentBrushChanged();
            }
            break;
        case IDC_SET:
            this_->event_handler_->OnSkyDialogBarSetToSkyClicked();
            break;
        case IDC_CLEAR:
            this_->event_handler_->OnSkyDialogBarClearSkyClicked();
            break;
        case IDC_CHOOSE:
            this_->event_handler_->OnSkyDialogBarChooseSkyTexClicked();
            break;
        }
        break;
    }

    return FALSE;
}

void SkyDialogBar::OnInitDialog()
{
    HWND combo = GetDlgItem(handle_, IDC_CURRENTBRUSH);
    ComboBox_AddString(combo, "Select");
    ComboBox_AddString(combo, "Insert Sky");
    ComboBox_AddString(combo, "Remove Sky");
    ComboBox_SetCurSel(combo, 0);
}

void SkyDialogBar::OnComboBoxCurrentBrushChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_CURRENTBRUSH);
    int cur_sel = ComboBox_GetCurSel(combo);

    EnableWindow(GetDlgItem(handle_, IDC_SET), cur_sel == 0);
    EnableWindow(GetDlgItem(handle_, IDC_CLEAR), cur_sel == 0);

    event_handler_->OnSkyDialogBarCurrentBrushChanged((SkyBrush)cur_sel);
}
