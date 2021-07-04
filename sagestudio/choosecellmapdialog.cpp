#include "choosecellmapdialog.h"
#include "resource.h"
#include "dialogutil.h"

#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseCellMapDialog *' of greater size

ChooseCellMapDialog::ChooseCellMapDialog()
: dialog_(NULL)
, cell_map_list_(NULL)
{
}

ChooseCellMapDialog::~ChooseCellMapDialog()
{
}

bool ChooseCellMapDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSECELLMAP_DIALOG), parent,
        ChooseCellMapDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseCellMapDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseCellMapDialog* this_ = reinterpret_cast<ChooseCellMapDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseCellMapDialog*>(lparam);
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
        case IDC_MAPS:
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




void ChooseCellMapDialog::OnInitDialog()
{
    HWND listbox = GetDlgItem(dialog_, IDC_MAPS);

    CellMapDocumentList::iterator itor;
    for(itor = cell_map_list_->begin(); itor != cell_map_list_->end(); ++itor)
    {
        ListBox_AddString(listbox, itor->c_str());
    }

    if(!cell_map_name_.empty())
    {
        ListBox_SelectString(listbox, -1, cell_map_name_.c_str());
    }
}

void ChooseCellMapDialog::OnButtonOK()
{
    HWND listbox = GetDlgItem(dialog_, IDC_MAPS);
    int cur_sel = ListBox_GetCurSel(listbox);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a CellMap", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_MAPS));
        return;
    }

    char buffer[MAX_PATH];
    ListBox_GetText(listbox, cur_sel, buffer);
    cell_map_name_ = buffer;

    EndDialog(dialog_, IDOK);
}
