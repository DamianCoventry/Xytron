#include "texturesetmanagerdialogbar.h"
#include "resource.h"

#include <stdexcept>
#include <commctrl.h>
#include <windowsx.h>

#include <sstream>

#include "../sagedocuments/texturesetdocument.h"

#include "dibresources.h"
#include "choosetexturedialog.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'TextureSetManagerDialogBar *' of greater size

TextureSetManagerDialogBar::TextureSetManagerDialogBar(HINSTANCE instance, HWND parent, TextureSetManagerDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_TEXTURESETMANAGER_DIALOGBAR), parent,
        TextureSetManagerDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create a TextureSetManagerDialogBar instance");
    }
}

TextureSetManagerDialogBar::~TextureSetManagerDialogBar()
{
    DestroyWindow(handle_);
    DeleteFont(od_font_);
}

void TextureSetManagerDialogBar::RepositionWithinParent()
{
    RECT rect;
    GetClientRect(GetParent(handle_), &rect);
    SetWindowPos(handle_, NULL, 200, 0, rect.right-200, rect.bottom, SWP_NOZORDER);

    GetClientRect(handle_, &rect);

    RECT list_box_rect;
    GetWindowRect(GetDlgItem(handle_, IDC_TEXTURESET), &list_box_rect);
    ScreenToClient(handle_, (POINT*)&list_box_rect.left);

    SetWindowPos(
        GetDlgItem(handle_, IDC_TEXTURESET), NULL, 0, 0,
        rect.right - 116, rect.bottom - 60,
        SWP_NOMOVE | SWP_NOZORDER);

    SetWindowPos(GetDlgItem(handle_, IDC_SET), NULL, rect.right-100, 8, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_REMOVE), NULL, rect.right-100, 38, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_CLEARALL), NULL, rect.right-100, 68, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    SetWindowPos(GetDlgItem(handle_, IDC_LIQUIDCOLOR_LABEL), NULL, 8, rect.bottom-50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    SetWindowPos(GetDlgItem(handle_, IDC_RED_LABEL), NULL, 8, rect.bottom-30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_RED), NULL, 48, rect.bottom-30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_GREEN_LABEL), NULL, 108, rect.bottom-30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_GREEN), NULL, 148, rect.bottom-30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_BLUE_LABEL), NULL, 208, rect.bottom-30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_BLUE), NULL, 248, rect.bottom-30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void TextureSetManagerDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void TextureSetManagerDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void TextureSetManagerDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
}

void TextureSetManagerDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void TextureSetManagerDialogBar::Clear()
{
    filenames_.clear();
    filenames_.resize(7);

    HWND list = GetDlgItem(handle_, IDC_TEXTURESET);
    ListBox_ResetContent(list);
    ListBox_AddString(list, "Wall");
    ListBox_AddString(list, "Ceiling");
    ListBox_AddString(list, "Floor");
    ListBox_AddString(list, "Liquid");
    ListBox_AddString(list, "Door");
    ListBox_AddString(list, "Translucent Wall");
    ListBox_AddString(list, "Translucent Door");
}

void TextureSetManagerDialogBar::SetTextureSetDocument(TextureSetDocument* doc)
{
    ts_doc_ = doc;
    filenames_.clear();
    filenames_.resize(7);

    HWND list = GetDlgItem(handle_, IDC_TEXTURESET);
    HDC dc = GetDC(handle_);

    if(!ts_doc_->TexWall().empty())
    {
        filenames_[TSC_WALL] = ts_doc_->TexWall();
        ListBox_SetItemData(list, TSC_WALL, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + ts_doc_->TexWall()));
    }
    if(!ts_doc_->TexCeiling().empty())
    {
        filenames_[TSC_CEILING] = ts_doc_->TexCeiling();
        ListBox_SetItemData(list, TSC_CEILING, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + ts_doc_->TexCeiling()));
    }
    if(!ts_doc_->TexFloor().empty())
    {
        filenames_[TSC_FLOOR] = ts_doc_->TexFloor();
        ListBox_SetItemData(list, TSC_FLOOR, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + ts_doc_->TexFloor()));
    }
    if(!ts_doc_->TexLiquid().empty())
    {
        filenames_[TSC_LIQUID] = ts_doc_->TexLiquid();
        ListBox_SetItemData(list, TSC_LIQUID, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + ts_doc_->TexLiquid()));
    }
    if(!ts_doc_->TexDoor().empty())
    {
        filenames_[TSC_DOOR] = ts_doc_->TexDoor();
        ListBox_SetItemData(list, TSC_DOOR, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + ts_doc_->TexDoor()));
    }
    if(!ts_doc_->TexTransWall().empty())
    {
        filenames_[TSC_TRANS_WALL] = ts_doc_->TexTransWall();
        ListBox_SetItemData(list, TSC_TRANS_WALL, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + ts_doc_->TexTransWall()));
    }
    if(!ts_doc_->TexTransDoor().empty())
    {
        filenames_[TSC_TRANS_DOOR] = ts_doc_->TexTransDoor();
        ListBox_SetItemData(list, TSC_TRANS_DOOR, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + ts_doc_->TexTransDoor()));
    }

    ReleaseDC(handle_, dc);

    std::ostringstream oss;
    oss << int(255.0f * ts_doc_->LiquidColor().r_);
    SetDlgItemText(handle_, IDC_RED, oss.str().c_str());

    oss.str("");
    oss << int(255.0f * ts_doc_->LiquidColor().g_);
    SetDlgItemText(handle_, IDC_GREEN, oss.str().c_str());

    oss.str("");
    oss << int(255.0f * ts_doc_->LiquidColor().b_);
    SetDlgItemText(handle_, IDC_BLUE, oss.str().c_str());
}





INT_PTR CALLBACK TextureSetManagerDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    TextureSetManagerDialogBar* this_ = reinterpret_cast<TextureSetManagerDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<TextureSetManagerDialogBar*>(lparam);
            this_->handle_ = dialog;
            this_->OnInitDialog();
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_MEASUREITEM:
        this_->OnListboxMeasureItem(reinterpret_cast<MEASUREITEMSTRUCT*>(lparam));
        break;
    case WM_DRAWITEM:
        this_->OnListboxDrawItem(reinterpret_cast<DRAWITEMSTRUCT*>(lparam));
        break;
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_TEXTURESET:
            if(HIWORD(wparam) == LBN_DBLCLK)
            {
                this_->OnButtonSet();
            }
            break;
        case IDC_SET:
            this_->OnButtonSet();
            break;
        case IDC_REMOVE:
            this_->OnButtonRemove();
            break;
        case IDC_CLEARALL:
            this_->OnButtonClearAll();
            break;
        case IDC_RED:
        case IDC_GREEN:
        case IDC_BLUE:
            if(HIWORD(wparam) == EN_CHANGE)
            {
                this_->OnEditBoxColorChanged();
            }
            break;
        }
        break;
    }

    return FALSE;
}

void TextureSetManagerDialogBar::OnInitDialog()
{
    HDC dc = GetDC(handle_);
    od_font_ = CreateFont(-MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
    ReleaseDC(handle_, dc);

    HWND list = GetDlgItem(handle_, IDC_TEXTURESET);
    ListBox_AddString(list, "Wall");
    ListBox_AddString(list, "Ceiling");
    ListBox_AddString(list, "Floor");
    ListBox_AddString(list, "Liquid");
    ListBox_AddString(list, "Door");
    ListBox_AddString(list, "Translucent Wall");
    ListBox_AddString(list, "Translucent Door");
}

void TextureSetManagerDialogBar::OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item)
{
    measure_item->itemHeight = 80;
}

void TextureSetManagerDialogBar::OnListboxDrawItem(DRAWITEMSTRUCT* draw_item)
{
    if(draw_item->itemID == -1)
    {
        return;
    }
    switch(draw_item->itemAction)
    {
    case ODA_SELECT: 
    case ODA_DRAWENTIRE: 
        {
            FillRect(draw_item->hDC, &draw_item->rcItem,
                GetSysColorBrush((draw_item->itemState & ODS_SELECTED) ? COLOR_HIGHLIGHT : COLOR_WINDOW));

            HDC mem_dc = CreateCompatibleDC(draw_item->hDC);
            HBITMAP prev_bmp = SelectBitmap(mem_dc, ListBox_GetItemData(draw_item->hwndItem, draw_item->itemID));

            BitBlt(draw_item->hDC, draw_item->rcItem.left + 8, draw_item->rcItem.top + 8, 64, 64, mem_dc, 0, 0, SRCCOPY);

            SelectBitmap(mem_dc, prev_bmp);
            DeleteDC(mem_dc);

            char path[MAX_PATH];
            ListBox_GetText(draw_item->hwndItem, draw_item->itemID, path);

            RECT rect;
            rect.top = draw_item->rcItem.top + 40;
            rect.left = draw_item->rcItem.left + 80;
            rect.bottom = draw_item->rcItem.bottom;
            rect.right = draw_item->rcItem.right;

            HFONT old_font      = SelectFont(draw_item->hDC, od_font_);
            int old_mode        = SetBkMode(draw_item->hDC, TRANSPARENT);
            COLORREF old_color  = SetTextColor(draw_item->hDC, (draw_item->itemState & ODS_SELECTED) ? RGB(255, 255, 255) : RGB(0, 0, 0));

            DrawText(draw_item->hDC, path, int(strlen(path)), &rect, DT_PATH_ELLIPSIS);

            SetTextColor(draw_item->hDC, old_color);
            SetBkMode(draw_item->hDC, old_mode);
            SelectFont(draw_item->hDC, old_font);

            break;
        }
    case ODA_FOCUS: 
        break;
    }
}

void TextureSetManagerDialogBar::OnButtonSet()
{
    HWND list = GetDlgItem(handle_, IDC_TEXTURESET);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel != LB_ERR)
    {
        ChooseTextureDialog dialog;
        dialog.DibResources(dib_resources_);
        dialog.ContentDirectory(content_dir_);
        if(dialog.Run(instance_, handle_))
        {
            HDC dc = GetDC(handle_);
            ListBox_SetItemData(list, cur_sel, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + dialog.Filename()));
            ReleaseDC(handle_, dc);

            InvalidateRect(list, NULL, TRUE);
            event_handler_->OnTextureSetSet((TextureSetChannel)cur_sel, dialog.Filename());
        }
    }
}

void TextureSetManagerDialogBar::OnButtonRemove()
{
    HWND list = GetDlgItem(handle_, IDC_TEXTURESET);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel != LB_ERR)
    {
        ListBox_SetItemData(list, cur_sel, 0);
        InvalidateRect(list, NULL, TRUE);
        event_handler_->OnTextureSetRemove((TextureSetChannel)cur_sel);
    }
}

void TextureSetManagerDialogBar::OnButtonClearAll()
{
    Clear();
    event_handler_->OnTextureSetClearAll();
}

void TextureSetManagerDialogBar::OnEditBoxColorChanged()
{
    UINT r, g, b;
    BOOL trans;

    r = GetDlgItemInt(handle_, IDC_RED, &trans, FALSE);
    if(trans)
    {
        g = GetDlgItemInt(handle_, IDC_GREEN, &trans, FALSE);
        if(trans)
        {
            b = GetDlgItemInt(handle_, IDC_BLUE, &trans, FALSE);
            if(trans)
            {
                Gfx::Color c;
                c.r_ = float(r)/255.0f;
                c.g_ = float(g)/255.0f;
                c.b_ = float(b)/255.0f;
                event_handler_->OnTextureSetSetLiquidColor(c);
            }
        }
    }
}
