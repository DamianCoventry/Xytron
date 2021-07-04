#include "lightsdialogbar.h"
#include "resource.h"
#include <stdexcept>
#include <commctrl.h>
#include <windowsx.h>
#include <boost/scoped_array.hpp>
#include "../sagedocuments/cellmapdocument.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'LightsDialogBar *' of greater size

LightsDialogBar::LightsDialogBar(HINSTANCE instance, HWND parent, LightsDialogBarEvents* event_handler, CellMapDocument* document)
: instance_(instance)
, event_handler_(event_handler)
, current_tool_(LT_SELECTLIGHT)
, document_(document)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_LIGHTS_DIALOGBAR), parent,
        LightsDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an LightsDialogBar instance");
    }
}

LightsDialogBar::~LightsDialogBar()
{
    DestroyWindow(handle_);
}

void LightsDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, parent_client_rect.right-200, 0, 200, parent_client_rect.bottom, SWP_NOZORDER);
}

void LightsDialogBar::InsertLight(const std::string& light)
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);
    ListBox_AddString(list, light.c_str());
}

void LightsDialogBar::SelectLights(const std::vector<std::string>& light_names)
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);
    if(light_names.empty())
    {
        ListBox_SetSel(list, FALSE, -1);
    }
    else
    {
        ListBox_SetSel(list, FALSE, -1);

        std::vector<std::string>::const_iterator itor;
        for(itor = light_names.begin(); itor != light_names.end(); ++itor)
        {
            int index = ListBox_FindString(list, -1, itor->c_str());
            if(index >= 0)
            {
                ListBox_SetSel(list, TRUE, index);
            }
        }

        Light* l = document_->GetLight(light_names.front());
        if(l)
        {
            SetDlgItemInt(handle_, IDC_LIGHTRADIUS, int(l->Radius()), FALSE);
            if(l->UseWorldAmbient())
            {
                CheckDlgButton(handle_, IDC_USEWORLD, BST_CHECKED);
                CheckDlgButton(handle_, IDC_SPECIFY, BST_UNCHECKED);
            }
            else
            {
                CheckDlgButton(handle_, IDC_USEWORLD, BST_UNCHECKED);
                CheckDlgButton(handle_, IDC_SPECIFY, BST_CHECKED);
            }
            SetDlgItemInt(handle_, IDC_AMBIENTLIGHT, l->Ambient(), FALSE);
        }
    }
}

void LightsDialogBar::SetLightRadius(int radius)
{
    SetDlgItemInt(handle_, IDC_LIGHTRADIUS, radius, FALSE);
}

void LightsDialogBar::SetUseWorldAmbient()
{
    CheckDlgButton(handle_, IDC_USEWORLD, BST_CHECKED);
    CheckDlgButton(handle_, IDC_SPECIFY, BST_UNCHECKED);
}

void LightsDialogBar::SetSpecificAmbient(int ambient)
{
    CheckDlgButton(handle_, IDC_USEWORLD, BST_UNCHECKED);
    CheckDlgButton(handle_, IDC_SPECIFY, BST_CHECKED);
    SetDlgItemInt(handle_, IDC_AMBIENTLIGHT, ambient, FALSE);
}

void LightsDialogBar::SetWorldAmbient(int ambient)
{
    SetDlgItemInt(handle_, IDC_AMBIENTWORLD, document_->AmbientLight(), FALSE);
}

void LightsDialogBar::OnLightRadiusChanged()
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);

    int sel_count = ListBox_GetSelCount(list);
    if(sel_count <= 0)
    {
        return;
    }

    int index;
    ListBox_GetSelItems(list, 1, &index);

    char buffer[128];
    ListBox_GetText(list, index, buffer);

    Light* l = document_->GetLight(buffer);
    if(l)
    {
        SetDlgItemInt(handle_, IDC_LIGHTRADIUS, int(l->Radius()), FALSE);
    }
}

void LightsDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void LightsDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void LightsDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
}

void LightsDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void LightsDialogBar::Clear()
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);
    ListBox_ResetContent(list);
    SetDlgItemText(handle_, IDC_LIGHTRADIUS, "");
    SetDlgItemText(handle_, IDC_AMBIENTLIGHT, "");
    SetDlgItemText(handle_, IDC_AMBIENTWORLD, "");
    current_tool_ = LT_SELECTLIGHT;
    HWND combo = GetDlgItem(handle_, IDC_CURRENTTOOL);
    ComboBox_SetCurSel(combo, current_tool_);
    CheckDlgButton(handle_, IDC_USEWORLD, BST_CHECKED);
}





INT_PTR CALLBACK LightsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    LightsDialogBar* this_ = reinterpret_cast<LightsDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<LightsDialogBar*>(lparam);
            this_->handle_ = dialog;
            this_->OnInitDialog();
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_LIGHTS:
            if(HIWORD(wparam) == LBN_SELCHANGE)
            {
                this_->OnCurrentLightChanged();
            }
            break;
        case IDC_INSERT:
            this_->OnButtonInsertLight();
            break;
        case IDC_REMOVE:
            this_->OnButtonRemoveLight();
            break;
        case IDC_VIEWLIGHT:
            this_->OnButtonViewLight();
            break;
        case IDC_SETLIGHTCOLOR:
            this_->OnButtonSetColor();
            break;
        case IDC_CURRENTTOOL:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnCurrentLightToolChanged();
            }
            break;
        case IDC_SETLIGHTRADIUS:
            this_->OnButtonSetLightRadius();
            break;
        case IDC_USEWORLD:
            this_->OnCheckBoxWorld();
            break;
        case IDC_SPECIFY:
            this_->OnCheckBoxSpecify();
            break;
        case IDC_SETSPECIFIC:
            this_->OnButtonSetAmbientSpecificLight();
            break;
        case IDC_SETWORLD:
            this_->OnButtonSetAmbientWorldLight();
            break;
        case IDC_CALCULATELIGHTING:
            this_->event_handler_->OnLightsDialogBarCalculateLighting();
            break;
        }
        break;
    }

    return FALSE;
}

void LightsDialogBar::OnInitDialog()
{
    HWND combo = GetDlgItem(handle_, IDC_CURRENTTOOL);
    ComboBox_AddString(combo, "Select Light");
    ComboBox_AddString(combo, "Cell Ambience");
    ComboBox_AddString(combo, "Light Radius");
    ComboBox_SetCurSel(combo, current_tool_);
    EnableDisableControls();
    CheckDlgButton(handle_, IDC_USEWORLD, BST_CHECKED);
    EnableWindow(GetDlgItem(handle_, IDC_AMBIENTLIGHT), FALSE);
}

void LightsDialogBar::OnCurrentLightChanged()
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);

    int sel_count = ListBox_GetSelCount(list);
    if(sel_count <= 0)
    {
        event_handler_->OnLightsDialogBarLightsSelected(std::vector<std::string>());
        return;
    }

    boost::scoped_array<int> indices(new int[sel_count]);
    sel_count = ListBox_GetSelItems(list, sel_count, indices.get());

    char buffer[128];
    std::vector<std::string> light_names;

    for(int i = 0; i < sel_count; i++)
    {
        ListBox_GetText(list, indices[i], buffer);
        light_names.push_back(buffer);
    }

    Light* l = document_->GetLight(light_names[0]);
    if(l)
    {
        SetDlgItemInt(handle_, IDC_LIGHTRADIUS, int(l->Radius()), FALSE);
        if(l->UseWorldAmbient())
        {
            CheckDlgButton(handle_, IDC_USEWORLD, BST_CHECKED);
            CheckDlgButton(handle_, IDC_SPECIFY, BST_UNCHECKED);
        }
        else
        {
            CheckDlgButton(handle_, IDC_USEWORLD, BST_UNCHECKED);
            CheckDlgButton(handle_, IDC_SPECIFY, BST_CHECKED);
        }
        SetDlgItemInt(handle_, IDC_AMBIENTLIGHT, l->Ambient(), FALSE);
    }

    event_handler_->OnLightsDialogBarLightsSelected(light_names);
}

void LightsDialogBar::OnButtonInsertLight()
{
    current_tool_ = LT_SELECTLIGHT;
    HWND combo = GetDlgItem(handle_, IDC_CURRENTTOOL);
    ComboBox_SetCurSel(combo, current_tool_);
    OnCurrentLightToolChanged();

    event_handler_->OnLightsDialogBarInsertLight();
}

void LightsDialogBar::OnButtonRemoveLight()
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);

    int sel_count = ListBox_GetSelCount(list);
    if(sel_count <= 0)
    {
        return;
    }

    boost::scoped_array<int> indices(new int[sel_count]);
    sel_count = ListBox_GetSelItems(list, sel_count, indices.get());

    char buffer[128];
    std::vector<std::string> light_names;

    for(int i = 0; i < sel_count; i++)
    {
        ListBox_GetText(list, indices[i], buffer);
        light_names.push_back(buffer);
    }

    if(light_names.empty())
    {
        return;
    }

    std::vector<std::string>::iterator itor;
    for(itor = light_names.begin(); itor != light_names.end(); ++itor)
    {
        int index = ListBox_FindString(list, -1, itor->c_str());
        if(index >= 0)
        {
            ListBox_DeleteString(list, index);
        }
    }

    event_handler_->OnLightsDialogBarRemoveLights(light_names);
}

void LightsDialogBar::OnButtonViewLight()
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);

    int sel_count = ListBox_GetSelCount(list);
    if(sel_count <= 0)
    {
        return;
    }

    int index;
    ListBox_GetSelItems(list, 1, &index);

    char buffer[128];
    ListBox_GetText(list, index, buffer);

    event_handler_->OnLightsDialogBarViewLight(buffer);
}

void LightsDialogBar::OnButtonSetColor()
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);

    int sel_count = ListBox_GetSelCount(list);
    if(sel_count <= 0)
    {
        return;
    }

    boost::scoped_array<int> indices(new int[sel_count]);
    sel_count = ListBox_GetSelItems(list, sel_count, indices.get());

    char buffer[128];
    std::vector<std::string> light_names;

    for(int i = 0; i < sel_count; i++)
    {
        ListBox_GetText(list, indices[i], buffer);
        light_names.push_back(buffer);
    }

    if(light_names.empty())
    {
        return;
    }

    event_handler_->OnLightsDialogBarSetSelectedLightsColor(light_names);
}

void LightsDialogBar::OnCurrentLightToolChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_CURRENTTOOL);
    current_tool_ = (LightTool)ComboBox_GetCurSel(combo);

    EnableDisableControls();

    event_handler_->OnLightsDialogBarCurrentLightToolChanged(current_tool_);
}

void LightsDialogBar::EnableDisableControls()
{
    switch(current_tool_)
    {
    case LT_SELECTLIGHT:
        EnableWindow(GetDlgItem(handle_, IDC_LIGHTRADIUSLABEL), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_LIGHTRADIUS), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_SETLIGHTRADIUS), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_USEWORLD), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_SPECIFY), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_AMBIENTLIGHT), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_STATIC2), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_SETSPECIFIC), FALSE);
        break;
    case LT_CELLAMBIENCE:
        EnableWindow(GetDlgItem(handle_, IDC_LIGHTRADIUSLABEL), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_LIGHTRADIUS), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_SETLIGHTRADIUS), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_USEWORLD), TRUE);
        EnableWindow(GetDlgItem(handle_, IDC_SPECIFY), TRUE);
        EnableWindow(GetDlgItem(handle_, IDC_AMBIENTLIGHT), IsDlgButtonChecked(handle_, IDC_SPECIFY) == BST_CHECKED);
        EnableWindow(GetDlgItem(handle_, IDC_STATIC2), IsDlgButtonChecked(handle_, IDC_SPECIFY) == BST_CHECKED);
        EnableWindow(GetDlgItem(handle_, IDC_SETSPECIFIC), IsDlgButtonChecked(handle_, IDC_SPECIFY) == BST_CHECKED);
        break;
    case LT_LIGHTRADIUS:
        EnableWindow(GetDlgItem(handle_, IDC_LIGHTRADIUSLABEL), TRUE);
        EnableWindow(GetDlgItem(handle_, IDC_LIGHTRADIUS), TRUE);
        EnableWindow(GetDlgItem(handle_, IDC_SETLIGHTRADIUS), TRUE);
        EnableWindow(GetDlgItem(handle_, IDC_USEWORLD), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_SPECIFY), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_AMBIENTLIGHT), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_STATIC2), FALSE);
        EnableWindow(GetDlgItem(handle_, IDC_SETSPECIFIC), FALSE);
        break;
    }
}

void LightsDialogBar::OnButtonSetLightRadius()
{
    HWND list = GetDlgItem(handle_, IDC_LIGHTS);

    int sel_count = ListBox_GetSelCount(list);
    if(sel_count <= 0)
    {
        return;
    }

    boost::scoped_array<int> indices(new int[sel_count]);
    sel_count = ListBox_GetSelItems(list, sel_count, indices.get());

    char buffer[128];
    std::vector<std::string> light_names;

    for(int i = 0; i < sel_count; i++)
    {
        ListBox_GetText(list, indices[i], buffer);
        light_names.push_back(buffer);
    }

    if(light_names.empty())
    {
        return;
    }

    BOOL trans;
    int i = GetDlgItemInt(handle_, IDC_LIGHTRADIUS, &trans, false);
    if(trans)
    {
        event_handler_->OnLightsDialogBarSetLightRadius(light_names, i);
    }
}

void LightsDialogBar::OnCheckBoxWorld()
{
    EnableWindow(GetDlgItem(handle_, IDC_AMBIENTLIGHT), FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_STATIC2), FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_SETSPECIFIC), FALSE);

    event_handler_->OnLightsDialogBarUseWorldAmbient();
}

void LightsDialogBar::OnCheckBoxSpecify()
{
    EnableWindow(GetDlgItem(handle_, IDC_AMBIENTLIGHT), TRUE);
    EnableWindow(GetDlgItem(handle_, IDC_STATIC2), TRUE);
    EnableWindow(GetDlgItem(handle_, IDC_SETSPECIFIC), TRUE);

    event_handler_->OnLightsDialogBarSpecifyAmbient();
}

void LightsDialogBar::OnButtonSetAmbientSpecificLight()
{
    BOOL trans;
    int i = GetDlgItemInt(handle_, IDC_AMBIENTLIGHT, &trans, false);
    if(trans)
    {
        event_handler_->OnLightsDialogBarSetAmbientSpecificLight(i);
    }
}

void LightsDialogBar::OnButtonSetAmbientWorldLight()
{
    BOOL trans;
    int i = GetDlgItemInt(handle_, IDC_AMBIENTWORLD, &trans, false);
    if(trans)
    {
        event_handler_->OnLightsDialogBarSetAmbientWorldLight(i);
    }
}
