#include "cellmapviewdialogbar.h"
#include "resource.h"

#include <stdexcept>
#include <boost/lexical_cast.hpp>

#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'CellMapViewDialogBar *' of greater size

CellMapViewDialogBar::CellMapViewDialogBar(HINSTANCE instance, HWND parent, CellMapViewDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_2DCELLMAP_VIEW_DIALOGBAR), parent,
        CellMapViewDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an CellMapViewDialogBar instance");
    }
}

CellMapViewDialogBar::~CellMapViewDialogBar()
{
    DestroyWindow(handle_);
}

void CellMapViewDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, 200, 0, parent_client_rect.right-400, 40, SWP_NOZORDER);
}

void CellMapViewDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void CellMapViewDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void CellMapViewDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void CellMapViewDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void CellMapViewDialogBar::Clear()
{
}

void CellMapViewDialogBar::SetViewStyle(ViewStyle vs)
{
    HWND combo = GetDlgItem(handle_, IDC_VIEWSTYLE);
    ComboBox_SetCurSel(combo, int(vs));
}

void CellMapViewDialogBar::SetEditMode(EditMode em)
{
    HWND combo = GetDlgItem(handle_, IDC_EDITMODE);
    ComboBox_SetCurSel(combo, int(em));
}

void CellMapViewDialogBar::SetZoomLevel(int zoom)
{
    HWND combo = GetDlgItem(handle_, IDC_ZOOMLEVEL);
    ComboBox_SetCurSel(combo, zoom);
}





INT_PTR CALLBACK CellMapViewDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    CellMapViewDialogBar* this_ = reinterpret_cast<CellMapViewDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<CellMapViewDialogBar*>(lparam);
            this_->handle_ = dialog;
            this_->OnInitDialog();
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_SHOWGRID:
            this_->event_handler_->OnCellMapViewDialogShowGridChanged(IsDlgButtonChecked(dialog, IDC_SHOWGRID) == BST_CHECKED);
            break;
        case IDC_VIEWSTYLE:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxViewStyleChanged();
            }
            break;
        case IDC_EDITMODE:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxEditModeChanged();
            }
            break;
        case IDC_ZOOMLEVEL:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxZoomLevelChanged();
            }
            break;
        }
        break;
    }

    return FALSE;
}

void CellMapViewDialogBar::OnInitDialog()
{
    CheckDlgButton(handle_, IDC_SHOWGRID, BST_CHECKED);

    HWND combo = GetDlgItem(handle_, IDC_VIEWSTYLE);
    ComboBox_AddString(combo, "Wireframe");
    ComboBox_AddString(combo, "Frontface Wireframe");
    ComboBox_AddString(combo, "Colored Polygons");
    ComboBox_AddString(combo, "Textured Polygons");
    ComboBox_SetCurSel(combo, 3);

    combo = GetDlgItem(handle_, IDC_ZOOMLEVEL);
    for(int i = 0; i < 10; i++)
    {
        ComboBox_AddString(combo, boost::lexical_cast<std::string>(i).c_str());
    }
    ComboBox_SetCurSel(combo, 3);

    combo = GetDlgItem(handle_, IDC_EDITMODE);
    ComboBox_AddString(combo, "Cell");
    ComboBox_AddString(combo, "Texture Set");
    ComboBox_AddString(combo, "Sound");
    ComboBox_AddString(combo, "Light");
    ComboBox_AddString(combo, "Entity");
    ComboBox_AddString(combo, "Camera");
    ComboBox_AddString(combo, "Region");
    ComboBox_AddString(combo, "Fog");
    ComboBox_AddString(combo, "Trigger");
    ComboBox_AddString(combo, "Sky");
    ComboBox_SetCurSel(combo, 0);
}

void CellMapViewDialogBar::OnComboBoxViewStyleChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_VIEWSTYLE);
    event_handler_->OnCellMapViewDialogBarViewStyleChanged((ViewStyle)ComboBox_GetCurSel(combo));
}

void CellMapViewDialogBar::OnComboBoxEditModeChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_EDITMODE);
    event_handler_->OnCellMapViewDialogBarEditModeChanged((EditMode)ComboBox_GetCurSel(combo));
}

void CellMapViewDialogBar::OnComboBoxZoomLevelChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_ZOOMLEVEL);
    event_handler_->OnCellMapViewDialogBarZoomLevelChanged(ComboBox_GetCurSel(combo));
}
