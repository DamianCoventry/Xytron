#include "resourcecontextdialogbar.h"
#include "resource.h"
#include <sstream>
#include <stdexcept>
#include <boost/scoped_array.hpp>
#include <commctrl.h>
#include <windowsx.h>
#include "dibresources.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ResourceContextDialogBar *' of greater size

ResourceContextDialogBar::ResourceContextDialogBar(HINSTANCE instance, HWND parent, ResourceContextDialogBarEvents* event_handler)
: instance_(instance)
, managing_(false)
, rc_doc_(NULL)
, img_doc_map_(NULL)
, img_anim_doc_map_(NULL)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_RESOURCECONTEXT_DIALOGBAR), parent,
        ResourceContextDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an ResourceContextDialogBar instance");
    }

    list_box_ = GetDlgItem(handle_, IDC_FILES);
}

ResourceContextDialogBar::~ResourceContextDialogBar()
{
    DeleteFont(od_font_);
    DestroyWindow(handle_);
}

void ResourceContextDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    int resources_dialog_bar_width = (managing_ ? 250 : 0);

    SetWindowPos(handle_, NULL, 200, 0,
        parent_client_rect.right-200, parent_client_rect.bottom, SWP_NOZORDER);

    GetClientRect(handle_, &parent_client_rect);



    RECT list_box_rect;
    GetWindowRect(list_box_, &list_box_rect);
    ScreenToClient(handle_, (POINT*)&list_box_rect.left);

    SetWindowPos(
        list_box_, NULL, 0, 0,
        parent_client_rect.right - (list_box_rect.left + 8 + resources_dialog_bar_width),
        parent_client_rect.bottom - (list_box_rect.top + 8),
        SWP_NOMOVE | SWP_NOZORDER);



    HWND control = GetDlgItem(handle_, IDC_MANAGE);
    RECT control_rect;
    GetWindowRect(control, &control_rect);
    ScreenToClient(handle_, (POINT*)&control_rect.left);
    ScreenToClient(handle_, (POINT*)&control_rect.right);

    SetWindowPos(
        control, NULL,
        parent_client_rect.right - ((control_rect.right-control_rect.left) + 8 + resources_dialog_bar_width), control_rect.top,
        0, 0, SWP_NOSIZE | SWP_NOZORDER);



    control = GetDlgItem(handle_, IDC_MOVE1LEFT);
    SetWindowPos(control, NULL, parent_client_rect.right - 246, 150, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    control = GetDlgItem(handle_, IDC_MOVEALLLEFT);
    SetWindowPos(control, NULL, parent_client_rect.right - 246, 180, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    control = GetDlgItem(handle_, IDC_MOVE1RIGHT);
    SetWindowPos(control, NULL, parent_client_rect.right - 246, 210, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    control = GetDlgItem(handle_, IDC_MOVEALLRIGHT);
    SetWindowPos(control, NULL, parent_client_rect.right - 246, 240, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    control = GetDlgItem(handle_, IDC_RESOURCETYPE);
    SetWindowPos(control, NULL, parent_client_rect.right - 210, 8, 200, 20, SWP_NOZORDER);

    control = GetDlgItem(handle_, IDC_RESOURCES);
    SetWindowPos(control, NULL, parent_client_rect.right - 210, 35, 200, parent_client_rect.bottom - 45, SWP_NOZORDER);

    control = GetDlgItem(handle_, IDC_ODRESOURCES);
    SetWindowPos(control, NULL, parent_client_rect.right - 210, 35, 200, parent_client_rect.bottom - 45, SWP_NOZORDER);
}

void ResourceContextDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void ResourceContextDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void ResourceContextDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void ResourceContextDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void ResourceContextDialogBar::RefreshResourceContextListBox()
{
    Clear();

    HDC dc = GetDC(handle_);
    MyTest::ResourceContextDocument::iterator rc_itor;
    for(rc_itor = rc_doc_->begin(); rc_itor != rc_doc_->end(); ++rc_itor)
    {
        int index = ListBox_AddString(list_box_, rc_itor->first.c_str());
        switch(rc_itor->second.type_)
        {
        case MyTest::RT_IMAGE:
            {
                ImageDocumentMap::iterator img_itor = img_doc_map_->find(rc_itor->first);
                if(img_itor != img_doc_map_->end())
                {
                    HBITMAP thumbnail_handle = dib_resources_->GetDibHandle("Image:" + rc_itor->first);
                    ListBox_SetItemData(list_box_, index, thumbnail_handle);
                }
                break;
            }
        case MyTest::RT_IMAGE_ANIMATION:
            {
                ImageAnimationDocumentMap::iterator img_anim_itor = img_anim_doc_map_->find(rc_itor->first);
                if(img_anim_itor != img_anim_doc_map_->end())
                {
                    HBITMAP thumbnail_handle = dib_resources_->GetDibHandle("ImageAnim:" + rc_itor->first);
                    ListBox_SetItemData(list_box_, index, thumbnail_handle);
                }
                break;
            }
        case MyTest::RT_TEXTURE:
            {
                HBITMAP thumbnail_handle = dib_resources_->GetDibHandle(content_dir_ + "\\Textures\\" + rc_itor->first);
                ListBox_SetItemData(list_box_, index, thumbnail_handle);
                break;
            }
        }
        rc_itor->second.list_box_item_ = index;
    }
    ReleaseDC(handle_, dc);
}

void ResourceContextDialogBar::RefreshResourceListBoxes()
{
    std::vector<std::string> filenames;

    HWND combo = GetDlgItem(handle_, IDC_RESOURCETYPE);
    switch(ComboBox_GetCurSel(combo))
    {
    case 0:         // Images
        {
            HDC dc = GetDC(handle_);

            HWND listbox = GetDlgItem(handle_, IDC_ODRESOURCES);
            ListBox_ResetContent(listbox);

            ImageDocumentMap::iterator itor;
            for(itor = img_doc_map_->begin(); itor != img_doc_map_->end(); ++itor)
            {
                if(!HasResourceBeenAdded(itor->first, MyTest::RT_IMAGE))
                {
                    int index = ListBox_AddString(listbox, itor->first.c_str());

                    HBITMAP thumbnail_handle = dib_resources_->GetDibHandle("Image:" + itor->first);
                    ListBox_SetItemData(listbox, index, thumbnail_handle);
                }
            }

            ReleaseDC(handle_, dc);
            break;
        }
    case 1:         // Image Animations
        {
            HDC dc = GetDC(handle_);

            HWND listbox = GetDlgItem(handle_, IDC_ODRESOURCES);
            ListBox_ResetContent(listbox);

            ImageAnimationDocumentMap::iterator itor;
            for(itor = img_anim_doc_map_->begin(); itor != img_anim_doc_map_->end(); ++itor)
            {
                if(!HasResourceBeenAdded(itor->first, MyTest::RT_IMAGE_ANIMATION))
                {
                    int index = ListBox_AddString(listbox, itor->first.c_str());

                    HBITMAP thumbnail_handle = dib_resources_->GetDibHandle("ImageAnim:" + itor->first);
                    ListBox_SetItemData(listbox, index, thumbnail_handle);
                }
            }

            ReleaseDC(handle_, dc);
            break;
        }
    case 2:         // Sounds
        {
            filenames = DirectoryListing("Sounds\\*.wav");

            HWND listbox = GetDlgItem(handle_, IDC_RESOURCES);
            ListBox_ResetContent(listbox);

            std::vector<std::string>::iterator itor;
            for(itor = filenames.begin(); itor != filenames.end(); ++itor)
            {
                if(!HasResourceBeenAdded(*itor, MyTest::RT_SOUND))
                {
                    int index = ListBox_AddString(listbox, itor->c_str());
                    ListBox_SetItemData(listbox, index, 0);
                }
            }
            break;
        }
    case 3:         // Music
        {
            filenames = DirectoryListing("Music\\*.mp3");

            HWND listbox = GetDlgItem(handle_, IDC_RESOURCES);
            ListBox_ResetContent(listbox);

            std::vector<std::string>::iterator itor;
            for(itor = filenames.begin(); itor != filenames.end(); ++itor)
            {
                if(!HasResourceBeenAdded(*itor, MyTest::RT_MUSIC))
                {
                    int index = ListBox_AddString(listbox, itor->c_str());
                    ListBox_SetItemData(listbox, index, 0);
                }
            }
            break;
        }
    }
}

std::vector<std::string> ResourceContextDialogBar::DirectoryListing(const std::string& filespec, bool use_content_dir)
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

bool ResourceContextDialogBar::HasResourceBeenAdded(const std::string& name, MyTest::ResourceType res_type) const
{
    MyTest::ResourceContextDocument::iterator itor = rc_doc_->find(name);
    if(itor == rc_doc_->end())
    {
        return false;
    }
    return (itor->second.type_ == res_type);
}

void ResourceContextDialogBar::ResourceContextDocument(MyTest::ResourceContextDocument* rc_doc)
{
    rc_doc_ = rc_doc;
    RefreshResourceContextListBox();
}

void ResourceContextDialogBar::Clear()
{
    ListBox_ResetContent(list_box_);
}




INT_PTR CALLBACK ResourceContextDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ResourceContextDialogBar* this_ = reinterpret_cast<ResourceContextDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<ResourceContextDialogBar*>(lparam);
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
        case IDC_MANAGE:
            this_->OnButtonManage();
            break;
        case IDC_RESOURCETYPE:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxResourceTypeChanged();
            }
            break;
        case IDC_MOVE1LEFT:
            this_->OnButtonMove1Left();
            break;
        case IDC_MOVEALLLEFT:
            this_->OnButtonMoveAllLeft();
            break;
        case IDC_MOVE1RIGHT:
            this_->OnButtonMove1Right();
            break;
        case IDC_MOVEALLRIGHT:
            this_->OnButtonMoveAllRight();
            break;
        case IDC_FILES:
            if(HIWORD(wparam) == LBN_DBLCLK)
            {
                this_->OnButtonMove1Right();
            }
            break;
        case IDC_RESOURCES:
            if(HIWORD(wparam) == LBN_DBLCLK)
            {
                this_->OnButtonMove1Left();
            }
            break;
        case IDC_ODRESOURCES:
            if(HIWORD(wparam) == LBN_DBLCLK)
            {
                this_->OnButtonMove1Left();
            }
            break;
        }
        break;
    }

    return FALSE;
}

void ResourceContextDialogBar::OnInitDialog()
{
    HDC dc = GetDC(handle_);
    od_font_ = CreateFont(-MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
    ReleaseDC(handle_, dc);

    HWND combo = GetDlgItem(handle_, IDC_RESOURCETYPE);
    ComboBox_AddString(combo, "Images");
    ComboBox_AddString(combo, "Image Animations");
    ComboBox_AddString(combo, "Sounds");
    ComboBox_AddString(combo, "Music");
    ComboBox_SetCurSel(combo, 0);

    OnComboBoxResourceTypeChanged();
}

void ResourceContextDialogBar::OnButtonManage()
{
    managing_ = !managing_;
    int show_cmd;

    if(managing_)
    {
        SetDlgItemText(handle_, IDC_MANAGE, "&Manage >>");
        show_cmd = SW_SHOW;
    }
    else
    {
        SetDlgItemText(handle_, IDC_MANAGE, "<< &Manage");
        show_cmd = SW_HIDE;
    }

    RepositionWithinParent();

    ShowWindow(GetDlgItem(handle_, IDC_MOVE1LEFT), show_cmd);
    ShowWindow(GetDlgItem(handle_, IDC_MOVEALLLEFT), show_cmd);
    ShowWindow(GetDlgItem(handle_, IDC_MOVE1RIGHT), show_cmd);
    ShowWindow(GetDlgItem(handle_, IDC_MOVEALLRIGHT), show_cmd);
    ShowWindow(GetDlgItem(handle_, IDC_RESOURCETYPE), show_cmd);
    ShowWindow(GetDlgItem(handle_, IDC_RESOURCES), show_cmd);
    ShowWindow(GetDlgItem(handle_, IDC_ODRESOURCES), show_cmd);

    OnComboBoxResourceTypeChanged();
}

void ResourceContextDialogBar::OnComboBoxResourceTypeChanged()
{
    if(managing_)
    {
        HWND combo = GetDlgItem(handle_, IDC_RESOURCETYPE);
        switch(ComboBox_GetCurSel(combo))
        {
        case 0:         // Images
            ShowWindow(GetDlgItem(handle_, IDC_RESOURCES), SW_HIDE);
            ShowWindow(GetDlgItem(handle_, IDC_ODRESOURCES), SW_SHOW);
            break;
        case 1:         // Image Animations
            ShowWindow(GetDlgItem(handle_, IDC_RESOURCES), SW_HIDE);
            ShowWindow(GetDlgItem(handle_, IDC_ODRESOURCES), SW_SHOW);
            break;
        case 2:         // Sounds
            ShowWindow(GetDlgItem(handle_, IDC_RESOURCES), SW_SHOW);
            ShowWindow(GetDlgItem(handle_, IDC_ODRESOURCES), SW_HIDE);
            break;
        case 3:         // Music
            ShowWindow(GetDlgItem(handle_, IDC_RESOURCES), SW_SHOW);
            ShowWindow(GetDlgItem(handle_, IDC_ODRESOURCES), SW_HIDE);
            break;
        }
        RefreshResourceListBoxes();
    }
}
void ResourceContextDialogBar::OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item)
{
    measure_item->itemHeight = 80;
}

void ResourceContextDialogBar::OnListboxDrawItem(DRAWITEMSTRUCT* draw_item)
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

void ResourceContextDialogBar::OnButtonMove1Left()
{
    HWND listbox;
    HWND combo = GetDlgItem(handle_, IDC_RESOURCETYPE);
    MyTest::ResourceType res_type;
    switch(ComboBox_GetCurSel(combo))
    {
    case 0:         // Images
        listbox = GetDlgItem(handle_, IDC_ODRESOURCES);
        res_type = MyTest::RT_IMAGE;
        break;
    case 1:         // Image Animations
        listbox = GetDlgItem(handle_, IDC_ODRESOURCES);
        res_type = MyTest::RT_IMAGE_ANIMATION;
        break;
    case 2:         // Sounds
        listbox = GetDlgItem(handle_, IDC_RESOURCES);
        res_type = MyTest::RT_SOUND;
        break;
    case 3:         // Music
        listbox = GetDlgItem(handle_, IDC_RESOURCES);
        res_type = MyTest::RT_MUSIC;
        break;
    }

    int count = ListBox_GetSelCount(listbox);
    if(count > 0)
    {
        boost::scoped_array<int> indicies(new int[count]);
        ListBox_GetSelItems(listbox, count, indicies.get());

        for(int i = 0; i < count; i++)
        {
            char buffer[128];
            ListBox_GetText(listbox, indicies[i], buffer);
            ULONG_PTR data = ListBox_GetItemData(listbox, indicies[i]);

            int index = ListBox_AddString(list_box_, buffer);
            ListBox_SetItemData(list_box_, index, data);

            MyTest::ResourceInfo info;
            info.type_          = res_type;
            info.list_box_item_ = index;
            rc_doc_->insert(std::make_pair(buffer, info));
        }

        RefreshResourceListBoxes();
        event_handler_->OnResourceContextModified();
    }
}

void ResourceContextDialogBar::OnButtonMoveAllLeft()
{
    HWND listbox;
    HWND combo = GetDlgItem(handle_, IDC_RESOURCETYPE);
    MyTest::ResourceType res_type;
    switch(ComboBox_GetCurSel(combo))
    {
    case 0:         // Images
        listbox = GetDlgItem(handle_, IDC_ODRESOURCES);
        res_type = MyTest::RT_IMAGE;
        break;
    case 1:         // Image Animations
        listbox = GetDlgItem(handle_, IDC_ODRESOURCES);
        res_type = MyTest::RT_IMAGE_ANIMATION;
        break;
    case 2:         // Sounds
        listbox = GetDlgItem(handle_, IDC_RESOURCES);
        res_type = MyTest::RT_SOUND;
        break;
    case 3:         // Music
        listbox = GetDlgItem(handle_, IDC_RESOURCES);
        res_type = MyTest::RT_MUSIC;
        break;
    }

    int count = ListBox_GetCount(listbox);
    if(count > 0)
    {
        for(int i = 0; i < count; i++)
        {
            char buffer[128];
            ListBox_GetText(listbox, i, buffer);
            ULONG_PTR data = ListBox_GetItemData(listbox, i);

            int index = ListBox_AddString(list_box_, buffer);
            ListBox_SetItemData(list_box_, index, data);

            MyTest::ResourceInfo info;
            info.type_          = res_type;
            info.list_box_item_ = index;
            rc_doc_->insert(std::make_pair(buffer, info));
        }

        RefreshResourceListBoxes();
        event_handler_->OnResourceContextModified();
    }
}

void ResourceContextDialogBar::OnButtonMove1Right()
{
    int count = ListBox_GetSelCount(list_box_);
    if(count > 0)
    {
        boost::scoped_array<int> indicies(new int[count]);
        ListBox_GetSelItems(list_box_, count, indicies.get());

        // Remove the selected items from the rc_doc_ object.  Don't remove
        // items of type RT_TEXTURE though.
        for(int i = 0; i < count; i++)
        {
            MyTest::ResourceContextDocument::iterator itor;
            for(itor = rc_doc_->begin(); itor != rc_doc_->end(); ++itor)
            {
                if(itor->second.type_ != MyTest::RT_TEXTURE && itor->second.list_box_item_ == indicies[i])
                {
                    rc_doc_->erase(itor);
                    break;
                }
            }
        }

        RefreshResourceContextListBox();
        RefreshResourceListBoxes();
        event_handler_->OnResourceContextModified();
    }
}

void ResourceContextDialogBar::OnButtonMoveAllRight()
{
    int count = ListBox_GetCount(list_box_);
    if(count > 0)
    {
        // Remove all items from rc_doc_ that are not type RT_TEXTURE
        MyTest::ResourceContextDocument::iterator i = rc_doc_->begin();
        while(i != rc_doc_->end())
        {
            if(i->second.type_ != MyTest::RT_TEXTURE)
            {
                rc_doc_->erase(i);
                i = rc_doc_->begin();
            }
            else
            {
                ++i;
            }
        }

        RefreshResourceContextListBox();
        RefreshResourceListBoxes();
        event_handler_->OnResourceContextModified();
    }
}
