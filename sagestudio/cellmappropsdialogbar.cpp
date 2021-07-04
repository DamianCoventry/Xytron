#include "cellmappropsdialogbar.h"
#include "resource.h"
#include <stdexcept>
#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'CellMapPropsDialogBar *' of greater size

CellMapPropsDialogBar::CellMapPropsDialogBar(HINSTANCE instance, HWND parent, CellMapPropsDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
, cell_brush_(CB_SELECT)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_2DCELLMAP_PROPS_DIALOGBAR), parent,
        CellMapPropsDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an CellMapPropsDialogBar instance");
    }
}

CellMapPropsDialogBar::~CellMapPropsDialogBar()
{
    DestroyWindow(handle_);
}

void CellMapPropsDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, parent_client_rect.right-200, 0, 200, parent_client_rect.bottom, SWP_NOZORDER);
}

void CellMapPropsDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void CellMapPropsDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void CellMapPropsDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void CellMapPropsDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void CellMapPropsDialogBar::Clear()
{
}

void CellMapPropsDialogBar::SetCellBrush(CellBrush brush)
{
    cell_brush_ = brush;
    HWND combo = GetDlgItem(handle_, IDC_BRUSH);
    switch(brush)
    {
    case CB_SELECT: ComboBox_SetCurSel(combo, 0); break;
    case CB_SOLID_SPACE: ComboBox_SetCurSel(combo, 1); break;
    case CB_NORMAL: ComboBox_SetCurSel(combo, 2); break;
    case CB_SECRET: ComboBox_SetCurSel(combo, 3); break;
    case CB_PIT: ComboBox_SetCurSel(combo, 4); break;
    case CB_LIQUID: ComboBox_SetCurSel(combo, 5); break;
    case CB_DOOR: ComboBox_SetCurSel(combo, 6); break;
    case CB_SECRET_DOOR: ComboBox_SetCurSel(combo, 7); break;
    case CB_TRANSLUCENT_DOOR: ComboBox_SetCurSel(combo, 8); break;
    case CB_TRANSLUCENT_SMALL_WALL: ComboBox_SetCurSel(combo, 9); break;
    case CB_TRANSLUCENT_LARGE_WALL: ComboBox_SetCurSel(combo, 10); break;
    }
}





INT_PTR CALLBACK CellMapPropsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    CellMapPropsDialogBar* this_ = reinterpret_cast<CellMapPropsDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<CellMapPropsDialogBar*>(lparam);
            this_->handle_ = dialog;
            this_->OnInitDialog();
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_BRUSH:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxCellBrushChanged();
            }
            break;
        case IDC_SELECTNONE:
            this_->event_handler_->OnCellMapSelectNone();
            break;
        case IDC_SETCELLTYPE:
            this_->event_handler_->OnCellMapSetCellType();
            break;
        case IDC_SETTEXTURESET:
            this_->event_handler_->OnCellMapSetTextureSet();
            break;
        }
        break;
    }

    return FALSE;
}

void CellMapPropsDialogBar::OnInitDialog()
{
    HWND combo = GetDlgItem(handle_, IDC_BRUSH);
    ComboBox_AddString(combo, "Select");
    ComboBox_AddString(combo, "Solid Space");
    ComboBox_AddString(combo, "Normal");
    ComboBox_AddString(combo, "Secret");
    ComboBox_AddString(combo, "Pit");
    ComboBox_AddString(combo, "Liquid");
    ComboBox_AddString(combo, "Door");
    ComboBox_AddString(combo, "Secret Door");
    ComboBox_AddString(combo, "Translucent Door");
    ComboBox_AddString(combo, "Translucent Small Wall");
    ComboBox_AddString(combo, "Translucent Large Wall");
    ComboBox_SetCurSel(combo, 0);
}

void CellMapPropsDialogBar::OnComboBoxCellBrushChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_BRUSH);
    int cur_sel = ComboBox_GetCurSel(combo);
    if(cur_sel != CB_ERR)
    {
        BOOL show = FALSE;
        CellBrush cell_brush;
        switch(cur_sel)
        {
        case 0: cell_brush = CB_SELECT; show = TRUE; break;
        case 1: cell_brush = CB_SOLID_SPACE; break;
        case 2: cell_brush = CB_NORMAL; break;
        case 3: cell_brush = CB_SECRET; break;
        case 4: cell_brush = CB_PIT; break;
        case 5: cell_brush = CB_LIQUID; break;
        case 6: cell_brush = CB_DOOR; break;
        case 7: cell_brush = CB_SECRET_DOOR; break;
        case 8: cell_brush = CB_TRANSLUCENT_DOOR; break;
        case 9: cell_brush = CB_TRANSLUCENT_SMALL_WALL; break;
        case 10: cell_brush = CB_TRANSLUCENT_LARGE_WALL; break;
        }
        event_handler_->OnCellMapCellBrushChanged(cell_brush);

        ShowWindow(GetDlgItem(handle_, IDC_SELECTNONE), show);
        ShowWindow(GetDlgItem(handle_, IDC_SELECTEDCELLS_GROUPBOX), show);
        ShowWindow(GetDlgItem(handle_, IDC_SETCELLTYPE), show);
        ShowWindow(GetDlgItem(handle_, IDC_SETTEXTURESET), show);
    }
}
