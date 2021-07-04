#include "contentdirectorydialog.h"
#include "resource.h"
#include "dialogutil.h"
#include "choosedirectorydialog.h"
#include "openfiledialog.h"
#include "dibresources.h"

#include <boost/scoped_array.hpp>
#include <sstream>

#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ContentDirectoryDialog *' of greater size

ContentDirectoryDialog::ContentDirectoryDialog()
: instance_(NULL)
, dialog_(NULL)
, audio_device_(NULL)
, dib_resources_(NULL)
{
}

ContentDirectoryDialog::~ContentDirectoryDialog()
{
    DeleteFont(od_font_);
    music_.Stop();
    music_.Unload();
}

bool ContentDirectoryDialog::Run(HINSTANCE instance, HWND parent)
{
    instance_ = instance;
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CONTENTDIRECTORY_DIALOG), parent,
        ContentDirectoryDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}

void ContentDirectoryDialog::RefreshListBox()
{
    std::vector<std::string> filenames;
    HDC dc = GetDC(dialog_);

    HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
    switch(ComboBox_GetCurSel(combo))
    {
    case 0:     // Textures
        {
            std::vector<std::string> temp(DirectoryListing("Textures\\*.bmp"));
            std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

            temp = DirectoryListing("Textures\\*.tga");
            std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

            temp = DirectoryListing("Textures\\*.png");
            std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

            HWND listbox = GetDlgItem(dialog_, IDC_ODTEXTURES);
            ListBox_ResetContent(listbox);

            std::vector<std::string>::iterator itor;
            for(itor = filenames.begin(); itor != filenames.end(); ++itor)
            {
                int index = ListBox_AddString(listbox, itor->c_str());
                ListBox_SetItemData(listbox, index, dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + *itor));
            }
            break;
        }
    case 1:     // Sounds
        {
            filenames = DirectoryListing("Sounds\\*.wav");

            HWND listbox = GetDlgItem(dialog_, IDC_FILES);
            ListBox_ResetContent(listbox);

            std::vector<std::string>::iterator itor;
            for(itor = filenames.begin(); itor != filenames.end(); ++itor)
            {
                ListBox_AddString(listbox, itor->c_str());
            }
            break;
        }
    case 2:     // Music
        {
            filenames = DirectoryListing("Music\\*.mp3");

            HWND listbox = GetDlgItem(dialog_, IDC_FILES);
            ListBox_ResetContent(listbox);

            std::vector<std::string>::iterator itor;
            for(itor = filenames.begin(); itor != filenames.end(); ++itor)
            {
                ListBox_AddString(listbox, itor->c_str());
            }
            break;
        }
    }

    ReleaseDC(dialog_, dc);
}

std::vector<std::string> ContentDirectoryDialog::DirectoryListing(const std::string& filespec, bool use_content_dir)
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




INT_PTR CALLBACK ContentDirectoryDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ContentDirectoryDialog* this_ = reinterpret_cast<ContentDirectoryDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ContentDirectoryDialog*>(lparam);
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
        case IDC_CHOOSECONTENTDIRECTORY:
            this_->OnButtonChooseContentDirectory();
            break;
        case IDC_CONTENTTYPE:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboBoxContentTypeChanged();
            }
            break;
        case IDC_ADDFILE:
            this_->OnButtonAddFile();
            break;
        case IDC_ADDDIRECTORY:
            this_->OnButtonAddDirectory();
            break;
        case IDC_DELETEFILE:
            this_->OnButtonDeleteFile();
            break;
        case IDC_DELETEALLFILES:
            this_->OnButtonDeleteAllFiles();
            break;
        case IDC_PLAYFILE:
            this_->OnButtonPlayFile();
            break;
        case IDC_STOPFILE:
            this_->OnButtonStopFile();
            break;
        case IDC_ODTEXTURES:
        case IDC_FILES:
            if(HIWORD(wparam) == LBN_SELCHANGE)
            {
                this_->OnListboxSelectionChanged();
            }
            break;
        case IDCANCEL:
            EndDialog(dialog, IDCANCEL);
            break;
        }
        break;
    }

    return FALSE;
}



    
void ContentDirectoryDialog::OnInitDialog()
{
    HDC dc = GetDC(dialog_);
    od_font_ = CreateFont(-MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72), 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
    ReleaseDC(dialog_, dc);

    HWND listbox = GetDlgItem(dialog_, IDC_ODTEXTURES);
    ListBox_SetColumnWidth(listbox, 256);

    HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
    ComboBox_AddString(combo, "Textures");
    ComboBox_AddString(combo, "Sound");
    ComboBox_AddString(combo, "Music");
    ComboBox_SetCurSel(combo, 0);

    if(!content_dir_.empty())
    {
        SetDlgItemText(dialog_, IDC_CONTENTDIRECTORY, content_dir_.c_str());
    }

    RefreshListBox();
    OnComboBoxContentTypeChanged();
    OnListboxSelectionChanged();
}

void ContentDirectoryDialog::OnButtonChooseContentDirectory()
{
    std::string directory = ChooseDirectoryDialog::Run(dialog_, "Please choose a directory that has sub-directories named \"Textures\", \"Sounds\" and \"Music\"");
    if(!directory.empty())
    {
        SetDlgItemText(dialog_, IDC_CONTENTDIRECTORY, directory.c_str());
    }
}

void ContentDirectoryDialog::OnComboBoxContentTypeChanged()
{
    HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
    switch(ComboBox_GetCurSel(combo))
    {
    case 0:     // Textures
        ShowWindow(GetDlgItem(dialog_, IDC_FILES), FALSE);
        ShowWindow(GetDlgItem(dialog_, IDC_ODTEXTURES), TRUE);
        EnableWindow(GetDlgItem(dialog_, IDC_PLAYFILE), FALSE);
        EnableWindow(GetDlgItem(dialog_, IDC_STOPFILE), FALSE);
        break;
    case 1:     // Sounds
        ShowWindow(GetDlgItem(dialog_, IDC_FILES), TRUE);
        ShowWindow(GetDlgItem(dialog_, IDC_ODTEXTURES), FALSE);
        EnableWindow(GetDlgItem(dialog_, IDC_PLAYFILE), TRUE);
        EnableWindow(GetDlgItem(dialog_, IDC_STOPFILE), FALSE);
        break;
    case 2:     // Music
        ShowWindow(GetDlgItem(dialog_, IDC_FILES), TRUE);
        ShowWindow(GetDlgItem(dialog_, IDC_ODTEXTURES), FALSE);
        EnableWindow(GetDlgItem(dialog_, IDC_PLAYFILE), TRUE);
        EnableWindow(GetDlgItem(dialog_, IDC_STOPFILE), TRUE);
        break;
    }

    RefreshListBox();
    OnListboxSelectionChanged();
}

void ContentDirectoryDialog::OnButtonAddFile()
{
    std::vector<std::string> source_files;

    HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
    int cb_cur_sel = ComboBox_GetCurSel(combo);
    switch(cb_cur_sel)
    {
    case 0:
        source_files = OpenFileDialog::GetMany(instance_, dialog_, "Texture Files (*.tga, *.bmp, *.png)\0*.tga;*.bmp;*.png\0All Files (*.*)\0*.*\0");
        break;
    case 1:
        source_files = OpenFileDialog::GetMany(instance_, dialog_, "Sound Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0");
        break;
    case 2:
        source_files = OpenFileDialog::GetMany(instance_, dialog_, "Music Files (*.mp3)\0*.mp3\0All Files (*.*)\0*.*\0");
        break;
    }

    if(!source_files.empty())
    {
        std::ostringstream dest_dir;
        dest_dir << content_dir_;

        switch(cb_cur_sel)
        {
        case 0: dest_dir << "\\Textures\\"; break;
        case 1: dest_dir << "\\Sounds\\"; break;
        case 2: dest_dir << "\\Music\\"; break;
        }

        std::vector<std::string>::iterator itor;
        for(itor = source_files.begin(); itor != source_files.end(); ++itor)
        {
            std::string src_filename;
            std::string::size_type pos = itor->find_last_of("\\");
            if(pos == std::string::npos)
            {
                pos = itor->find_last_of("/");
                if(pos == std::string::npos)
                {
                    src_filename = *itor;
                }
                else
                {
                    src_filename = itor->substr(pos+1);
                }
            }
            else
            {
                src_filename = itor->substr(pos+1);
            }

            std::string dest_filename = dest_dir.str() + src_filename;
            CopyFile(itor->c_str(), dest_filename.c_str(), FALSE);
        }
        RefreshListBox();
    }
}

void ContentDirectoryDialog::OnButtonAddDirectory()
{
    std::ostringstream oss;
    oss << "Choose a directory whose ";

    HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
    int cb_cur_sel = ComboBox_GetCurSel(combo);
    switch(cb_cur_sel)
    {
    case 0: oss << "TGA, BMP and PNG"; break;
    case 1: oss << "WAV"; break;
    case 2: oss << "MP3"; break;
    }
    oss << " files you want to copy to the Content Directory";

    std::string directory = ChooseDirectoryDialog::Run(dialog_, oss.str());
    if(!directory.empty())
    {
        std::vector<std::string> filenames;
        std::string dest_directory = content_dir_;

        switch(cb_cur_sel)
        {
        case 0:     // Textures
            {
                oss.str("");
                oss << directory << "\\*.bmp";
                std::vector<std::string> temp(DirectoryListing(oss.str(), false));
                std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

                oss.str("");
                oss << directory << "\\*.tga";
                temp = DirectoryListing(oss.str(), false);
                std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

                oss.str("");
                oss << directory << "\\*.png";
                temp = DirectoryListing(oss.str(), false);
                std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

                dest_directory += "\\Textures\\";
                break;
            }
        case 1:     // Sounds
            oss.str("");
            oss << directory << "\\*.wav";
            filenames = DirectoryListing(oss.str(), false);
            dest_directory += "\\Sounds\\";
            break;
        case 2:     // Music
            oss.str("");
            oss << directory << "\\*.mp3";
            filenames = DirectoryListing(oss.str(), false);
            dest_directory += "\\Music\\";
            break;
        }

        directory += "\\";
        std::vector<std::string>::iterator itor;
        for(itor = filenames.begin(); itor != filenames.end(); ++itor)
        {
            std::string existing_filename = directory + *itor;
            std::string new_filename = dest_directory + *itor;
            CopyFile(existing_filename.c_str(), new_filename.c_str(), FALSE);
        }

        RefreshListBox();
    }
}

void ContentDirectoryDialog::OnButtonDeleteFile()
{
    HWND listbox;
    HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
    int cb_cur_sel = ComboBox_GetCurSel(combo);
    switch(cb_cur_sel)
    {
    case 0:     // Textures
        listbox = GetDlgItem(dialog_, IDC_ODTEXTURES);
        break;
    case 1:     // Sounds
    case 2:     // Music
        listbox = GetDlgItem(dialog_, IDC_FILES);
        break;
    }

    int count = ListBox_GetSelCount(listbox);
    if(count > 0)
    {
        if(MessageBox(dialog_, "The selected files will be permanently deleted from the disk\r\n\r\nAre you sure you want to do this?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDNO)
        {
            return;
        }

        std::ostringstream oss;
        oss << content_dir_;

        switch(cb_cur_sel)
        {
        case 0: oss << "\\Textures\\"; break;
        case 1: oss << "\\Sounds\\"; break;
        case 2: oss << "\\Music\\"; break;
        }

        boost::scoped_array<int> indicies(new int[count]);
        ListBox_GetSelItems(listbox, count, indicies.get());

        for(int i = 0; i < count; i++)
        {
            char buffer[MAX_PATH];
            ListBox_GetText(listbox, indicies[i], buffer);
            std::string full_path = oss.str() + buffer;
            DeleteFile(full_path.c_str());
        }

        RefreshListBox();
    }
}

void ContentDirectoryDialog::OnButtonDeleteAllFiles()
{
    if(MessageBox(dialog_, "All files will be permanently deleted from the disk\r\n\r\nAre you sure you want to do this?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDNO)
    {
        return;
    }

    std::ostringstream oss;
    oss << content_dir_;

    HWND listbox;
    HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
    int cb_cur_sel = ComboBox_GetCurSel(combo);
    switch(cb_cur_sel)
    {
    case 0:
        listbox = GetDlgItem(dialog_, IDC_ODTEXTURES);
        oss << "\\Textures\\";
        break;
    case 1:
        listbox = GetDlgItem(dialog_, IDC_FILES);
        oss << "\\Sounds\\";
        break;
    case 2:
        listbox = GetDlgItem(dialog_, IDC_FILES);
        oss << "\\Music\\";
        break;
    }

    int count = ListBox_GetCount(listbox);
    for(int i = 0; i < count; i++)
    {
        char buffer[MAX_PATH];
        ListBox_GetText(listbox, i, buffer);
        std::string full_path = oss.str() + buffer;
        DeleteFile(full_path.c_str());
    }

    RefreshListBox();
}

void ContentDirectoryDialog::OnButtonPlayFile()
{
    HWND listbox = GetDlgItem(dialog_, IDC_FILES);
    int count = ListBox_GetSelCount(listbox);
    if(count > 0)
    {
        boost::scoped_array<int> indicies(new int[count]);
        ListBox_GetSelItems(listbox, count, indicies.get());

        char buffer[MAX_PATH];
        ListBox_GetText(listbox, indicies[0], buffer);

        HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
        switch(ComboBox_GetCurSel(combo))
        {
        case 1:
            {
                try
                {
                    Aud::WaveFile file;
                    file.Load(content_dir_ + "\\Sounds\\" + buffer);

                    sound_ = audio_device_->CreateSound(file.GetBytes(), file.GetLength(), 1.0f, 5.0f);
                    sound_->Play2d();
                }
                catch(std::exception& )
                {
                    MessageBox(dialog_, "Couldn't play sound file", "Information", MB_ICONINFORMATION | MB_OK);
                }
                break;
            }
        case 2:
            music_.Stop();
            music_.Unload();

            music_.Load(content_dir_ + "\\Music\\" + buffer, dialog_, 1234);
            music_.Play();
            break;
        }
    }
}

void ContentDirectoryDialog::OnButtonStopFile()
{
    music_.Stop();
    music_.Unload();
}

void ContentDirectoryDialog::OnListboxSelectionChanged()
{
    HWND listbox = GetDlgItem(dialog_, IDC_FILES);
    int count = ListBox_GetSelCount(listbox);

    HWND combo = GetDlgItem(dialog_, IDC_CONTENTTYPE);
    int cb_cur_sel = ComboBox_GetCurSel(combo);
    switch(cb_cur_sel)
    {
    case 0:     // Textures
        break;
    case 1:     // Sounds
        EnableWindow(GetDlgItem(dialog_, IDC_PLAYFILE), count > 0);
        break;
    case 2:     // Music
        EnableWindow(GetDlgItem(dialog_, IDC_PLAYFILE), count > 0);
        EnableWindow(GetDlgItem(dialog_, IDC_STOPFILE), count > 0);
        break;
    }
}




void ContentDirectoryDialog::OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item)
{
    measure_item->itemHeight = 80;
}

void ContentDirectoryDialog::OnListboxDrawItem(DRAWITEMSTRUCT* draw_item)
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
