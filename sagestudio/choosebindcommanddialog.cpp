#include "choosebindcommanddialog.h"
#include "resource.h"
#include "dialogutil.h"

#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#include "choosecameradialog.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseBindCommandDialog *' of greater size

ChooseBindCommandDialog::ChooseBindCommandDialog()
: instance_(NULL)
, dialog_(NULL)
{
}

bool ChooseBindCommandDialog::Run(HINSTANCE instance, HWND parent)
{
    instance_ = instance;
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSEBINDCOMMAND_DIALOG), parent,
        ChooseBindCommandDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseBindCommandDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseBindCommandDialog* this_ = reinterpret_cast<ChooseBindCommandDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseBindCommandDialog*>(lparam);
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
        case IDC_BINDCOMMANDS:
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




void ChooseBindCommandDialog::OnInitDialog()
{
    HWND list = GetDlgItem(dialog_, IDC_BINDCOMMANDS);

    // Insert all the items.  In this case the items are our available game commands.
    ListBox_AddString(list, "Camera.Move.Forward");
    ListBox_AddString(list, "Camera.Move.Backward");
    ListBox_AddString(list, "Camera.Move.Left");
    ListBox_AddString(list, "Camera.Move.Right");
    ListBox_AddString(list, "Camera.Move.Up");
    ListBox_AddString(list, "Camera.Move.Down");
    ListBox_AddString(list, "Camera.Look");
    ListBox_AddString(list, "Camera.Current.Set");
    ListBox_AddString(list, "Camera.Current.First");
    ListBox_AddString(list, "Camera.Current.Last");
    ListBox_AddString(list, "Camera.Current.Next");
    ListBox_AddString(list, "Camera.Current.Prev");
    ListBox_AddString(list, "Camera.ToggleCollision");

    // Select the given command
    if(!cl_.empty())
    {
        ListBox_SelectString(list, -1, cl_[0].c_str());
    }
}

void ChooseBindCommandDialog::OnButtonOK()
{
    HWND list = GetDlgItem(dialog_, IDC_BINDCOMMANDS);

    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a bind command", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_BINDCOMMANDS));
        return;
    }

    char buffer[128];
    ListBox_GetText(list, cur_sel, buffer);

    cl_.clear();
    cl_.push_back(buffer);

    if(cl_[0] == "Camera.Current.Set")
    {
        ChooseCameraDialog d;
        d.CameraDocuments(cam_docs_);
        if(d.Run(instance_, dialog_))
        {
            cl_.push_back(d.Camera());
        }
        else
        {
            return;     // the user cancelled the choose camera dialog.
        }
    }

    EndDialog(dialog_, IDOK);
}
