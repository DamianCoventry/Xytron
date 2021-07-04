#include "choosesounddialog.h"
#include "resource.h"
#include "dialogutil.h"

#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseSoundDialog *' of greater size

ChooseSoundDialog::ChooseSoundDialog()
: dialog_(NULL)
{
}

ChooseSoundDialog::~ChooseSoundDialog()
{
}

bool ChooseSoundDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSESOUND_DIALOG), parent,
        ChooseSoundDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseSoundDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseSoundDialog* this_ = reinterpret_cast<ChooseSoundDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseSoundDialog*>(lparam);
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
        case IDC_SOUNDS:
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




void ChooseSoundDialog::OnInitDialog()
{
    HWND listbox = GetDlgItem(dialog_, IDC_SOUNDS);

    WIN32_FIND_DATA find_data;
    memset(&find_data, 0, sizeof(WIN32_FIND_DATA));

    std::ostringstream oss;
    oss << content_dir_ << "\\Sounds\\*.wav";

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

    if(!sound_filename_.empty())
    {
        ListBox_SelectString(listbox, -1, sound_filename_.c_str());
    }
}

void ChooseSoundDialog::OnButtonOK()
{
    HWND listbox = GetDlgItem(dialog_, IDC_SOUNDS);
    int cur_sel = ListBox_GetCurSel(listbox);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a Sound", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_SOUNDS));
        return;
    }

    char buffer[MAX_PATH];
    ListBox_GetText(listbox, cur_sel, buffer);
    sound_filename_ = buffer;

    EndDialog(dialog_, IDOK);
}
