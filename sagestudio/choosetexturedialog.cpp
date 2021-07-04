#include "chooseTexturedialog.h"
#include "resource.h"
#include "dialogutil.h"
#include "dibresources.h"

#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseTextureDialog *' of greater size

ChooseTextureDialog::ChooseTextureDialog()
: dialog_(NULL)
{
}

ChooseTextureDialog::~ChooseTextureDialog()
{
    DeleteFont(od_font_);
}

bool ChooseTextureDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSETEXTURE_DIALOG), parent,
        ChooseTextureDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseTextureDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseTextureDialog* this_ = reinterpret_cast<ChooseTextureDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseTextureDialog*>(lparam);
            this_->dialog_ = dialog;
            this_->OnInitDialog();

            return TRUE;        // Let the system call SetFocus()
        }
    case WM_CLOSE:
        EndDialog(dialog, IDCANCEL);
        break;
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
            if(HIWORD(wparam) == LBN_DBLCLK)
            {
                this_->OnButtonOK();
            }
            break;
        case IDOK:
            this_->OnButtonOK();
            break;
        case IDCANCEL:
            EndDialog(dialog, IDCANCEL);
            break;
        }
        break;
    }

    return FALSE;
}




void ChooseTextureDialog::OnInitDialog()
{
    HDC dc = GetDC(dialog_);
    od_font_ = CreateFont(-MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

    std::vector<std::string> filenames;

    std::vector<std::string> temp(DirectoryListing("Textures\\*.bmp"));
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    temp = DirectoryListing("Textures\\*.tga");
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    temp = DirectoryListing("Textures\\*.png");
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    HWND list = GetDlgItem(dialog_, IDC_TEXTURES);
    ListBox_ResetContent(list);

    std::vector<std::string>::iterator itor;
    for(itor = filenames.begin(); itor != filenames.end(); ++itor)
    {
        int index = ListBox_AddString(list, itor->c_str());
        ListBox_SetItemData(list, index, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + *itor));
    }

    ReleaseDC(dialog_, dc);

    if(!filename_.empty())
    {
        ListBox_SelectString(list, -1, filename_.c_str());
    }
}

void ChooseTextureDialog::OnButtonOK()
{
    HWND list = GetDlgItem(dialog_, IDC_TEXTURES);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a Texture", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_TEXTURES));
        return;
    }

    char buffer[MAX_PATH];
    ListBox_GetText(list, cur_sel, buffer);
    filename_ = buffer;

    EndDialog(dialog_, IDOK);
}

void ChooseTextureDialog::OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item)
{
    measure_item->itemHeight    = 80;
    measure_item->itemWidth     = 200;
}

void ChooseTextureDialog::OnListboxDrawItem(DRAWITEMSTRUCT* draw_item)
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

std::vector<std::string> ChooseTextureDialog::DirectoryListing(const std::string& filespec, bool use_content_dir)
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
