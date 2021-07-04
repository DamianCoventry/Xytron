#include "imagepropertiesdialogbar.h"
#include "resource.h"
#include "dibresources.h"
#include <stdexcept>
#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ImagePropertiesDialogBar *' of greater size

ImagePropertiesDialogBar::ImagePropertiesDialogBar(HINSTANCE instance, HWND parent, ImagePropertiesDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_IMAGE_PROPS_DIALOGBAR), parent,
        ImagePropertiesDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an ImagePropertiesDialogBar instance");
    }

    list_box_ = GetDlgItem(handle_, IDC_TEXTURES);
}

ImagePropertiesDialogBar::~ImagePropertiesDialogBar()
{
    DeleteFont(od_font_);
    DestroyWindow(handle_);
}

void ImagePropertiesDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);
    SetWindowPos(handle_, NULL, parent_client_rect.right-300, 0, 300, parent_client_rect.bottom, SWP_NOZORDER);

    GetClientRect(handle_, &parent_client_rect);
    SetWindowPos(list_box_, NULL, 10, 10, parent_client_rect.right-20, parent_client_rect.bottom-230, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_LABELFRAMEX), NULL, 10, parent_client_rect.bottom-210, 50, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_FRAMEX), NULL, 70, parent_client_rect.bottom-210, 120, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_LABELFRAMEY), NULL, 10, parent_client_rect.bottom-175, 50, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_FRAMEY), NULL, 70, parent_client_rect.bottom-175, 120, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_LABELFRAMEW), NULL, 10, parent_client_rect.bottom-140, 50, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_FRAMEW), NULL, 70, parent_client_rect.bottom-140, 120, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_LABELFRAMEH), NULL, 10, parent_client_rect.bottom-105, 50, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_FRAMEH), NULL, 70, parent_client_rect.bottom-105, 120, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_LABELFRAMECOUNT), NULL, 10, parent_client_rect.bottom-70, 50, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_FRAMECOUNT), NULL, 70, parent_client_rect.bottom-70, 120, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_APPLYPOS), NULL, 200, parent_client_rect.bottom-195, 75, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_APPLYSIZE), NULL, 200, parent_client_rect.bottom-125, 75, 24, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_APPLYFRAMECOUNT), NULL, 200, parent_client_rect.bottom-70, 75, 24, SWP_NOZORDER);
}

void ImagePropertiesDialogBar::Texture(const std::string& filename)
{
    int index = ListBox_FindString(list_box_, -1, filename.c_str());
    if(index != LB_ERR)
    {
        ListBox_SetSel(list_box_, TRUE, index);
    }
}

void ImagePropertiesDialogBar::Position(const POINT& position)
{
    SetDlgItemInt(handle_, IDC_FRAMEX, position.x, TRUE);
    SetDlgItemInt(handle_, IDC_FRAMEY, position.y, TRUE);
}

void ImagePropertiesDialogBar::Size(const SIZE& size)
{
    SetDlgItemInt(handle_, IDC_FRAMEW, size.cx, TRUE);
    SetDlgItemInt(handle_, IDC_FRAMEH, size.cy, TRUE);
}

void ImagePropertiesDialogBar::FrameCount(int frame_count)
{
    SetDlgItemInt(handle_, IDC_FRAMECOUNT, frame_count, TRUE);
}

void ImagePropertiesDialogBar::EnableFrameCount(bool enable)
{
    EnableWindow(GetDlgItem(handle_, IDC_LABELFRAMECOUNT), enable ? TRUE : FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_FRAMECOUNT), enable ? TRUE : FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_APPLYFRAMECOUNT), enable ? TRUE : FALSE);
}

void ImagePropertiesDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void ImagePropertiesDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void ImagePropertiesDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void ImagePropertiesDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void ImagePropertiesDialogBar::Clear()
{
}

void ImagePropertiesDialogBar::RefreshListBox()
{
    std::vector<std::string> filenames;

    std::vector<std::string> temp(DirectoryListing("Textures\\*.bmp"));
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    temp = DirectoryListing("Textures\\*.tga");
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    temp = DirectoryListing("Textures\\*.png");
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    ListBox_ResetContent(list_box_);

    HDC dc = GetDC(handle_);

    std::vector<std::string>::iterator itor;
    for(itor = filenames.begin(); itor != filenames.end(); ++itor)
    {
        int index = ListBox_AddString(list_box_, itor->c_str());
        ListBox_SetItemData(list_box_, index, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + *itor));
    }

    ReleaseDC(handle_, dc);
}

std::vector<std::string> ImagePropertiesDialogBar::DirectoryListing(const std::string& filespec, bool use_content_dir)
{
    std::vector<std::string> rv;

    std::ostringstream oss;
    if(use_content_dir)
    {
        oss << content_dir_ << "\\" << filespec;
    }
    else
    {
        oss << filespec;
    }

    WIN32_FIND_DATA find_data;
    memset(&find_data, 0, sizeof(WIN32_FIND_DATA));

    HANDLE find_handle = FindFirstFile(oss.str().c_str(), &find_data);
    if(find_handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                rv.push_back(find_data.cFileName);
            }
        }
        while(FindNextFile(find_handle, &find_data));

        FindClose(find_handle);
    }

    return rv;
}





INT_PTR CALLBACK ImagePropertiesDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ImagePropertiesDialogBar* this_ = reinterpret_cast<ImagePropertiesDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<ImagePropertiesDialogBar*>(lparam);
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
        case IDC_TEXTURES:
            if(HIWORD(wparam) == LBN_SELCHANGE)
            {
                this_->OnListBoxTextureChanged();
            }
            break;
        case IDC_APPLYPOS:
            this_->OnButtonApplyPosition();
            break;
        case IDC_APPLYSIZE:
            this_->OnButtonApplySize();
            break;
        case IDC_APPLYFRAMECOUNT:
            this_->OnButtonApplyFrameCount();
            break;
        }
        break;
    }

    return FALSE;
}

void ImagePropertiesDialogBar::OnInitDialog()
{
    HDC dc = GetDC(handle_);
    od_font_ = CreateFont(-MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
    ReleaseDC(handle_, dc);

    RefreshListBox();
}

void ImagePropertiesDialogBar::OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item)
{
    measure_item->itemHeight = 80;
}

void ImagePropertiesDialogBar::OnListboxDrawItem(DRAWITEMSTRUCT* draw_item)
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

void ImagePropertiesDialogBar::OnListBoxTextureChanged()
{
    int cur_sel = ListBox_GetCurSel(list_box_);
    if(cur_sel != LB_ERR)
    {
        event_handler_->OnImagePropertiesDialogBarModified();
        char buffer[128];
        ListBox_GetText(list_box_, cur_sel, buffer);
        event_handler_->OnImagePropertiesDialogBarTextureChanged(buffer);
    }
}

void ImagePropertiesDialogBar::OnButtonApplyPosition()
{
    BOOL translated;
    int x_coord = GetDlgItemInt(handle_, IDC_FRAMEX, &translated, TRUE);
    if(translated)
    {
        int y_coord = GetDlgItemInt(handle_, IDC_FRAMEY, &translated, TRUE);
        if(translated)
        {
            event_handler_->OnImagePropertiesDialogBarModified();
            POINT position;
            position.x = x_coord;
            position.y = y_coord;
            event_handler_->OnImagePropertiesDialogBarPositionChanged(position);
        }
    }
}

void ImagePropertiesDialogBar::OnButtonApplySize()
{
    BOOL translated;
    int width = GetDlgItemInt(handle_, IDC_FRAMEW, &translated, TRUE);
    if(translated)
    {
        int height = GetDlgItemInt(handle_, IDC_FRAMEH, &translated, TRUE);
        if(translated)
        {
            event_handler_->OnImagePropertiesDialogBarModified();
            SIZE size;
            size.cx = width;
            size.cy = height;
            event_handler_->OnImagePropertiesDialogBarSizeChanged(size);
        }
    }
}

void ImagePropertiesDialogBar::OnButtonApplyFrameCount()
{
    BOOL translated;
    int frame_count = GetDlgItemInt(handle_, IDC_FRAMECOUNT, &translated, TRUE);
    if(translated)
    {
        event_handler_->OnImagePropertiesDialogBarModified();
        event_handler_->OnImageDialogFrameCountChanged(frame_count);
    }
}
