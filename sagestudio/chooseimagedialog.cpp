#include "chooseimagedialog.h"
#include "resource.h"
#include "dialogutil.h"
#include "dibresources.h"

#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseImageDialog *' of greater size

ChooseImageDialog::ChooseImageDialog()
: dialog_(NULL)
, image_anim_map_(NULL)
, image_map_(NULL)
{
}

ChooseImageDialog::~ChooseImageDialog()
{
    DeleteFont(od_font_);
}

bool ChooseImageDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSEIMAGE_DIALOG), parent,
        ChooseImageDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseImageDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseImageDialog* this_ = reinterpret_cast<ChooseImageDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseImageDialog*>(lparam);
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
        case IDC_IMAGES:
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




void ChooseImageDialog::OnInitDialog()
{
    HDC dc = GetDC(dialog_);
    od_font_ = CreateFont(-MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
    ReleaseDC(dialog_, dc);

    HWND listbox = GetDlgItem(dialog_, IDC_IMAGES);

    if(image_anim_map_)
    {
        SetWindowText(dialog_, "Choose an Image Animation");

        ImageAnimationDocumentMap::iterator img_anim_itor;
        for(img_anim_itor = image_anim_map_->begin(); img_anim_itor != image_anim_map_->end(); ++img_anim_itor)
        {
            int index = ListBox_AddString(listbox, img_anim_itor->first.c_str());
            ListBox_SetItemData(listbox, index, dib_resources_->GetDibHandle("ImageAnim:" + img_anim_itor->first));
        }
    }

    if(image_map_)
    {
        SetWindowText(dialog_, "Choose an Image");

        ImageDocumentMap::iterator img_itor;
        for(img_itor = image_map_->begin(); img_itor != image_map_->end(); ++img_itor)
        {
            int index = ListBox_AddString(listbox, img_itor->first.c_str());
            ListBox_SetItemData(listbox, index, dib_resources_->GetDibHandle("Image:" + img_itor->first));
        }
    }

    if(!image_filename_.empty())
    {
        ListBox_SelectString(listbox, -1, image_filename_.c_str());
    }
}

void ChooseImageDialog::OnButtonOK()
{
    HWND listbox = GetDlgItem(dialog_, IDC_IMAGES);
    int cur_sel = ListBox_GetCurSel(listbox);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select an image", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_IMAGES));
        return;
    }

    char buffer[MAX_PATH];
    ListBox_GetText(listbox, cur_sel, buffer);
    image_filename_ = buffer;

    EndDialog(dialog_, IDOK);
}

void ChooseImageDialog::OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item)
{
    measure_item->itemHeight = 80;
}

void ChooseImageDialog::OnListboxDrawItem(DRAWITEMSTRUCT* draw_item)
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
