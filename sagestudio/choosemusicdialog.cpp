#include "choosemusicdialog.h"
#include "resource.h"
#include "dialogutil.h"

#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseMusicDialog *' of greater size

ChooseMusicDialog::ChooseMusicDialog()
: dialog_(NULL)
{
}

ChooseMusicDialog::~ChooseMusicDialog()
{
}

bool ChooseMusicDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSEMUSIC_DIALOG), parent,
        ChooseMusicDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseMusicDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseMusicDialog* this_ = reinterpret_cast<ChooseMusicDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseMusicDialog*>(lparam);
            this_->dialog_ = dialog;
            this_->OnInitDialog();

            return TRUE;        // Let the system call SetFocus()
        }
    case WM_CLOSE:
        EndDialog(dialog, IDCANCEL);
        break;
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_MUSIC:
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




void ChooseMusicDialog::OnInitDialog()
{
    HWND listbox = GetDlgItem(dialog_, IDC_MUSIC);

    WIN32_FIND_DATA find_data;
    memset(&find_data, 0, sizeof(WIN32_FIND_DATA));

    std::ostringstream oss;
    oss << content_dir_ << "\\Music\\*.mp3";

    HANDLE find_handle = FindFirstFile(oss.str().c_str(), &find_data);
    if(find_handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                ListBox_AddString(listbox, find_data.cFileName);
            }
        }
        while(FindNextFile(find_handle, &find_data));

        FindClose(find_handle);
    }

    if(!music_filename_.empty())
    {
        ListBox_SelectString(listbox, -1, music_filename_.c_str());
    }
}

void ChooseMusicDialog::OnButtonOK()
{
    HWND listbox = GetDlgItem(dialog_, IDC_MUSIC);
    int cur_sel = ListBox_GetCurSel(listbox);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a Music", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_MUSIC));
        return;
    }

    char buffer[MAX_PATH];
    ListBox_GetText(listbox, cur_sel, buffer);
    music_filename_ = buffer;

    EndDialog(dialog_, IDOK);
}
