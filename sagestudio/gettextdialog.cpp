#include "gettextdialog.h"
#include "resource.h"
#include "dialogutil.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'GetTextDialog *' of greater size

bool GetTextDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_GETTEXT_DIALOG), parent,
        GetTextDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK GetTextDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    GetTextDialog* this_ = reinterpret_cast<GetTextDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<GetTextDialog*>(lparam);
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




void GetTextDialog::OnInitDialog()
{
    if(!title_.empty())
    {
        SetWindowText(dialog_, title_.c_str());
    }
    if(!label_.empty())
    {
        SetDlgItemText(dialog_, IDC_LABEL, label_.c_str());
    }
    if(!text_.empty())
    {
        SetDlgItemText(dialog_, IDC_TEXT, text_.c_str());
    }
}

void GetTextDialog::OnButtonOK()
{
    char buffer[128];
    if(GetDlgItemText(dialog_, IDC_TEXT, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter some text", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_TEXT));
        return;
    }
    text_ = buffer;
    EndDialog(dialog_, IDOK);
}
