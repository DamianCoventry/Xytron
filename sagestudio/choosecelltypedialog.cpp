#include "choosecelltypedialog.h"
#include "resource.h"
#include "dialogutil.h"
#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseCellTypeDialog *' of greater size

bool ChooseCellTypeDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSECELLTYPE_DIALOG), parent,
        ChooseCellTypeDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseCellTypeDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseCellTypeDialog* this_ = reinterpret_cast<ChooseCellTypeDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseCellTypeDialog*>(lparam);
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
        case IDC_CELLTYPE:
            if(HIWORD(wparam) == LBN_DBLCLK)
            {
                this_->OnButtonOK();
                break;
            }
        }
        break;
    }

    return FALSE;
}




void ChooseCellTypeDialog::OnInitDialog()
{
    HWND list = GetDlgItem(dialog_, IDC_CELLTYPE);

    ListBox_AddString(list, "Solid Space");
    ListBox_AddString(list, "Normal");
    ListBox_AddString(list, "Secret");
    ListBox_AddString(list, "Pit");
    ListBox_AddString(list, "Liquid");
    ListBox_AddString(list, "Door");
    ListBox_AddString(list, "Secret Door");
    ListBox_AddString(list, "Translucent Door");
    ListBox_AddString(list, "Translucent Small Wall");
    ListBox_AddString(list, "Translucent Large Wall");

    ListBox_SetCurSel(list, cell_type_);
}

void ChooseCellTypeDialog::OnButtonOK()
{
    HWND list = GetDlgItem(dialog_, IDC_CELLTYPE);

    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a Cell Type", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_CELLTYPE));
        return;
    }

    cell_type_ = (CellType)cur_sel;

    EndDialog(dialog_, IDOK);
}
