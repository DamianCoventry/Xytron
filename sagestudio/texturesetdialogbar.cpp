#include "texturesetdialogbar.h"
#include "resource.h"
#include <stdexcept>
#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'TextureSetDialogBar *' of greater size

TextureSetDialogBar::TextureSetDialogBar(HINSTANCE instance, HWND parent, TextureSetDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_TEXTURESET_DIALOGBAR), parent,
        TextureSetDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create a TextureSetDialogBar instance");
    }
}

TextureSetDialogBar::~TextureSetDialogBar()
{
    DestroyWindow(handle_);
}

void TextureSetDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, parent_client_rect.right-200, 0, 200, parent_client_rect.bottom, SWP_NOZORDER);
}

void TextureSetDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void TextureSetDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void TextureSetDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
}

void TextureSetDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void TextureSetDialogBar::Clear()
{
}

void TextureSetDialogBar::SetTextureSetDocumentMap(TextureSetDocumentMap* ts_docs)
{
    ts_docs_ = ts_docs;

    HWND list = GetDlgItem(handle_, IDC_TEXTURESETS);
    ListBox_ResetContent(list);

    HWND combo = GetDlgItem(handle_, IDC_DEFAULTTEXTURESET);
    ComboBox_ResetContent(combo);

    TextureSetDocumentMap::iterator i;
    for(i = ts_docs_->begin(); i != ts_docs_->end(); ++i)
    {
        ListBox_AddString(list, i->first.c_str());
        ComboBox_AddString(combo, i->first.c_str());
    }

    if(default_texture_set_.empty())
    {
        ComboBox_SetCurSel(combo, 0);
    }
    else
    {
        int index = ComboBox_SelectString(combo, -1, default_texture_set_.c_str());
        if(index == CB_ERR)
        {
            ComboBox_SetCurSel(combo, 0);
        }
    }

    char buffer[128];
    ComboBox_GetText(combo, buffer, 128);
    default_texture_set_ = buffer;
    event_handler_->OnTextureSetDefaultTextureSetChanged(default_texture_set_);
}





INT_PTR CALLBACK TextureSetDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    TextureSetDialogBar* this_ = reinterpret_cast<TextureSetDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<TextureSetDialogBar*>(lparam);
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
                this_->OnComboBoxTextureSetBrushChanged();
            }
            break;
        case IDC_SELECTNONE:
            this_->event_handler_->OnTextureSetSelectNone();
            break;
        case IDC_SETCELLTYPE:
            this_->event_handler_->OnTextureSetSetCellType();
            break;
        case IDC_SETTEXTURESET:
            this_->event_handler_->OnTextureSetSetTextureSet();
            break;
        case IDC_TEXTURESETS:
            if(HIWORD(wparam) == LBN_SELCHANGE)
            {
                this_->OnListBoxTextureSetChanged();
            }
            break;
        case IDC_DEFAULTTEXTURESET:
            this_->OnComboBoxDefaultTextureSetChanged();
            break;
        }
        break;
    }

    return FALSE;
}

void TextureSetDialogBar::OnInitDialog()
{
    HWND combo = GetDlgItem(handle_, IDC_BRUSH);
    ComboBox_AddString(combo, "Select");
    ComboBox_AddString(combo, "Apply");
    ComboBox_SetCurSel(combo, 0);

    OnComboBoxTextureSetBrushChanged();
}

void TextureSetDialogBar::OnComboBoxTextureSetBrushChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_BRUSH);
    int cur_sel = ComboBox_GetCurSel(combo);
    if(cur_sel != CB_ERR)
    {
        BOOL show = FALSE;
        TextureSetBrush ts_brush;
        switch(cur_sel)
        {
        case 0: ts_brush = TSB_SELECT; show = TRUE; break;
        case 1: ts_brush = TSB_APPLY; break;
        }
        event_handler_->OnTextureSetBrushChanged(ts_brush);

        ShowWindow(GetDlgItem(handle_, IDC_SELECTNONE), show);
        ShowWindow(GetDlgItem(handle_, IDC_SELECTEDCELLS_GROUPBOX), show);
        ShowWindow(GetDlgItem(handle_, IDC_SETCELLTYPE), show);
        ShowWindow(GetDlgItem(handle_, IDC_SETTEXTURESET), show);

        ShowWindow(GetDlgItem(handle_, IDC_TEXTURESETS), !show);
        ShowWindow(GetDlgItem(handle_, IDC_MANAGE), !show);
    }
}

void TextureSetDialogBar::OnListBoxTextureSetChanged()
{
    HWND list = GetDlgItem(handle_, IDC_TEXTURESETS);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel != LB_ERR)
    {
        char buffer[128];
        ListBox_GetText(list, cur_sel, buffer);
        event_handler_->OnTextureSetTextureSetChanged(buffer);
    }
}

void TextureSetDialogBar::OnComboBoxDefaultTextureSetChanged()
{
    HWND combo = GetDlgItem(handle_, IDC_DEFAULTTEXTURESET);
    char buffer[128];
    ComboBox_GetText(combo, buffer, 128);
    default_texture_set_ = buffer;
    event_handler_->OnTextureSetDefaultTextureSetChanged(default_texture_set_);
}
