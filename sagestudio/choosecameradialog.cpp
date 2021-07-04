#include "choosecameradialog.h"
#include "resource.h"
#include "dialogutil.h"

#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseCameraDialog *' of greater size

ChooseCameraDialog::ChooseCameraDialog()
: dialog_(NULL)
{
}

ChooseCameraDialog::~ChooseCameraDialog()
{
}

bool ChooseCameraDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSECAMERA_DIALOG), parent,
        ChooseCameraDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseCameraDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseCameraDialog* this_ = reinterpret_cast<ChooseCameraDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseCameraDialog*>(lparam);
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
        case IDC_CAMERAS:
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




void ChooseCameraDialog::OnInitDialog()
{
    HWND list = GetDlgItem(dialog_, IDC_CAMERAS);

    CameraDocumentMap::iterator i;
    for(i = docs_->begin(); i != docs_->end(); ++i)
    {
        ListBox_AddString(list, i->first.c_str());
    }

    ListBox_SelectString(list, -1, camera_.c_str());
}

void ChooseCameraDialog::OnButtonOK()
{
    HWND list = GetDlgItem(dialog_, IDC_CAMERAS);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a Camera", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_CAMERAS));
        return;
    }

    char buffer[MAX_PATH];
    ListBox_GetText(list, cur_sel, buffer);
    camera_ = buffer;

    EndDialog(dialog_, IDOK);
}
