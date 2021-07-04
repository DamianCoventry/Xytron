#include "savemapprogressdialog.h"
#include "resource.h"
#include "dialogutil.h"
#include <commctrl.h>
#include <windowsx.h>

#include <boost/bind.hpp>
#include "../sagedocuments/cellmapfile.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'SaveMapProgressDialog *' of greater size

bool SaveMapProgressDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_SAVEMAPPROGRESS_DIALOG), parent,
        SaveMapProgressDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK SaveMapProgressDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    SaveMapProgressDialog* this_ = reinterpret_cast<SaveMapProgressDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<SaveMapProgressDialog*>(lparam);
            this_->dialog_ = dialog;

            SetTimer(dialog, 0, 100, NULL);
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_CLOSE:
        EndDialog(dialog, IDCANCEL);
        break;
    case WM_TIMER:
        this_->OnWindowTimer();
        break;
    }

    return FALSE;
}




void SaveMapProgressDialog::OnWindowTimer()
{
    KillTimer(dialog_, 0);

    SetDlgItemText(dialog_, IDC_STATIC1, "Saving Cell Map");

    CellMapFile file;
    if(!file.Save(path_, document_,
        boost::bind(&SaveMapProgressDialog::OnCellMapFileTotal, this, _1),
        boost::bind(&SaveMapProgressDialog::OnCellMapFileStep, this, _1)))
    {
        EndDialog(dialog_, IDCANCEL);
        return;
    }

    EndDialog(dialog_, IDOK);
}

void SaveMapProgressDialog::OnCellMapFileTotal(int total)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS1);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, total));
    SendMessage(progress, PBM_SETPOS, 0, 0);
}

void SaveMapProgressDialog::OnCellMapFileStep(int value)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS1);
    SendMessage(progress, PBM_SETPOS, value, 0);
}
