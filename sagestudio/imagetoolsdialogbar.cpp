#include "imagetoolsdialogbar.h"
#include "resource.h"

#include <stdexcept>
#include <sstream>

#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ImageToolsDialogBar *' of greater size

ImageToolsDialogBar::ImageToolsDialogBar(HINSTANCE instance, HWND parent, ImageToolsDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
, zoom_(100)
, tool_(ImageToolsDialogBarEvents::T_MOVEIMAGE)
, grid_snap_enabled_(true)
, grid_snap_(10)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_IMAGE_TOOLS_DIALOGBAR), parent,
        ImageToolsDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an ImageToolsDialogBar instance");
    }
}

ImageToolsDialogBar::~ImageToolsDialogBar()
{
    DestroyWindow(handle_);
}

void ImageToolsDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, 200, 0, parent_client_rect.right-500, 40, SWP_NOZORDER);
}

void ImageToolsDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void ImageToolsDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void ImageToolsDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void ImageToolsDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void ImageToolsDialogBar::Clear()
{
}

void ImageToolsDialogBar::Zoom(int zoom)
{
    zoom_ = (zoom / 25) * 25;;

    if(zoom_ < 50) zoom_ = 50;
    else if(zoom_ > 500) zoom_= 500;

    std::ostringstream oss;
    oss << zoom << "%";

    HWND combo = GetDlgItem(handle_, IDC_ZOOM);
    ComboBox_SelectString(combo, -1, oss.str().c_str());
}

void ImageToolsDialogBar::SetTool(ImageToolsDialogBarEvents::Tool tool)
{
    tool_ = tool;
    HWND combo = GetDlgItem(handle_, IDC_TOOL);
    switch(tool_)
    {
    case ImageToolsDialogBarEvents::T_MOVEIMAGE:
        ComboBox_SetCurSel(combo, 0);
        break;
    case ImageToolsDialogBarEvents::T_RESIZEIMAGE:
        ComboBox_SetCurSel(combo, 1);
        break;
    case ImageToolsDialogBarEvents::T_MOVEVIEW:
        ComboBox_SetCurSel(combo, 2);
        break;
    }
}

void ImageToolsDialogBar::EnableGridSnap(bool enable)
{
    grid_snap_enabled_ = enable;
    CheckDlgButton(handle_, IDC_ENABLEGRIDSNAP, grid_snap_enabled_ ? BST_CHECKED : BST_UNCHECKED);
}

void ImageToolsDialogBar::GridSnap(int grid_snap)
{
    grid_snap_ = grid_snap;
    SetDlgItemInt(handle_, IDC_GRIDSNAP, grid_snap_, TRUE);
}





INT_PTR CALLBACK ImageToolsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ImageToolsDialogBar* this_ = reinterpret_cast<ImageToolsDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<ImageToolsDialogBar*>(lparam);
            this_->handle_ = dialog;
            this_->OnInitDialog();
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_ZOOM:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxZoomChanged();
            }
            break;
        case IDC_TOOL:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxToolChanged();
            }
            break;
        case IDC_ENABLEGRIDSNAP:
            this_->OnCheckBoxEnableGridSnap();
            break;
        case IDC_APPLY:
            this_->OnButtonApplyGridSnap();
            break;
        }
        break;
    }

    return FALSE;
}

void ImageToolsDialogBar::OnInitDialog()
{
    int index;
    HWND combo = GetDlgItem(handle_, IDC_ZOOM);
    index = ComboBox_AddString(combo, "50%");
    ComboBox_SetItemData(combo, index, 50);
    index = ComboBox_AddString(combo, "75%");
    ComboBox_SetItemData(combo, index, 75);
    index = ComboBox_AddString(combo, "100%");
    ComboBox_SetItemData(combo, index, 100);
    index = ComboBox_AddString(combo, "125%");
    ComboBox_SetItemData(combo, index, 125);
    index = ComboBox_AddString(combo, "150%");
    ComboBox_SetItemData(combo, index, 150);
    index = ComboBox_AddString(combo, "175%");
    ComboBox_SetItemData(combo, index, 175);
    index = ComboBox_AddString(combo, "200%");
    ComboBox_SetItemData(combo, index, 200);
    index = ComboBox_AddString(combo, "225%");
    ComboBox_SetItemData(combo, index, 225);
    index = ComboBox_AddString(combo, "250%");
    ComboBox_SetItemData(combo, index, 250);
    index = ComboBox_AddString(combo, "275%");
    ComboBox_SetItemData(combo, index, 275);
    index = ComboBox_AddString(combo, "300%");
    ComboBox_SetItemData(combo, index, 300);
    index = ComboBox_AddString(combo, "325%");
    ComboBox_SetItemData(combo, index, 325);
    index = ComboBox_AddString(combo, "350%");
    ComboBox_SetItemData(combo, index, 350);
    index = ComboBox_AddString(combo, "375%");
    ComboBox_SetItemData(combo, index, 375);
    index = ComboBox_AddString(combo, "400%");
    ComboBox_SetItemData(combo, index, 400);
    index = ComboBox_AddString(combo, "425%");
    ComboBox_SetItemData(combo, index, 425);
    index = ComboBox_AddString(combo, "470%");
    ComboBox_SetItemData(combo, index, 470);
    index = ComboBox_AddString(combo, "475%");
    ComboBox_SetItemData(combo, index, 475);
    index = ComboBox_AddString(combo, "500%");
    ComboBox_SetItemData(combo, index, 500);
    ComboBox_SetCurSel(combo, 2);       // 100%

    combo = GetDlgItem(handle_, IDC_TOOL);
    ComboBox_AddString(combo, "Move Image");
    ComboBox_AddString(combo, "Resize Image");
    ComboBox_AddString(combo, "Move View");
    ComboBox_SetCurSel(combo, 0);       // Move Image

    CheckDlgButton(handle_, IDC_ENABLEGRIDSNAP, BST_CHECKED);
    SetDlgItemInt(handle_, IDC_GRIDSNAP, grid_snap_, TRUE);
}

void ImageToolsDialogBar::OnComboBoxZoomChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_ZOOM);
    int cur_sel = ComboBox_GetCurSel(combo);
    if(cur_sel != CB_ERR)
    {
        zoom_ = int(ComboBox_GetItemData(combo, cur_sel));
        event_handler_->OnImageToolsDialogBarZoomChanged(zoom_);
    }
}

void ImageToolsDialogBar::OnComboBoxToolChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_TOOL);
    int cur_sel = ComboBox_GetCurSel(combo);
    if(cur_sel != CB_ERR)
    {
        switch(cur_sel)
        {
        case 0: tool_ = ImageToolsDialogBarEvents::T_MOVEIMAGE; break;
        case 1: tool_ = ImageToolsDialogBarEvents::T_RESIZEIMAGE; break;
        case 2: tool_ = ImageToolsDialogBarEvents::T_MOVEVIEW; break;
        }
        event_handler_->OnImageToolsDialogBarToolChanged(tool_);
    }
}

void ImageToolsDialogBar::OnCheckBoxEnableGridSnap()
{
    grid_snap_enabled_ = (IsDlgButtonChecked(handle_, IDC_ENABLEGRIDSNAP) == BST_CHECKED);

    EnableWindow(GetDlgItem(handle_, IDC_GRIDSNAP), grid_snap_enabled_ ? TRUE : FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_APPLY), grid_snap_enabled_ ? TRUE : FALSE);

    event_handler_->OnImageToolsDialogBarEnableGridSnapChanged(grid_snap_enabled_);
}

void ImageToolsDialogBar::OnButtonApplyGridSnap()
{
    BOOL translated;
    grid_snap_ = GetDlgItemInt(handle_, IDC_GRIDSNAP, &translated, TRUE);
    if(translated)
    {
        event_handler_->OnImageToolsDialogBarGridSnapChanged(grid_snap_);
    }
}
