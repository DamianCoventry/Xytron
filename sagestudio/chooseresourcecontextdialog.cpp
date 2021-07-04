#include "chooseresourcecontextdialog.h"
#include "resource.h"
#include "dialogutil.h"

#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseResourceContextDialog *' of greater size

ChooseResourceContextDialog::ChooseResourceContextDialog()
: dialog_(NULL)
, res_ctxt_map_(NULL)
{
}

ChooseResourceContextDialog::~ChooseResourceContextDialog()
{
}

bool ChooseResourceContextDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSERESOURCECONTEXT_DIALOG), parent,
        ChooseResourceContextDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseResourceContextDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseResourceContextDialog* this_ = reinterpret_cast<ChooseResourceContextDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseResourceContextDialog*>(lparam);
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
        case IDC_RESOURCECONTEXTS:
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




void ChooseResourceContextDialog::OnInitDialog()
{
    HWND listbox = GetDlgItem(dialog_, IDC_RESOURCECONTEXTS);

    MyTest::ResourceContextDocumentMap::iterator itor;
    for(itor = res_ctxt_map_->begin(); itor != res_ctxt_map_->end(); ++itor)
    {
        ListBox_AddString(listbox, itor->first.c_str());
    }

    if(!resource_context_name_.empty())
    {
        ListBox_SelectString(listbox, -1, resource_context_name_.c_str());
    }
}

void ChooseResourceContextDialog::OnButtonOK()
{
    HWND listbox = GetDlgItem(dialog_, IDC_RESOURCECONTEXTS);
    int cur_sel = ListBox_GetCurSel(listbox);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a ResourceContext", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_RESOURCECONTEXTS));
        return;
    }

    char buffer[MAX_PATH];
    ListBox_GetText(listbox, cur_sel, buffer);
    resource_context_name_ = buffer;

    EndDialog(dialog_, IDOK);
}
