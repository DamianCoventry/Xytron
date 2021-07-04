#include "lightingprogressdialog.h"
#include "resource.h"
#include "dialogutil.h"
#include <commctrl.h>
#include <windowsx.h>

#include <boost/bind.hpp>
#include "../sagedocuments/cellmapdocument.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'LightingProgressDialog *' of greater size

bool LightingProgressDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_LIGHTINGPROGRESS_DIALOG), parent,
        LightingProgressDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK LightingProgressDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    LightingProgressDialog* this_ = reinterpret_cast<LightingProgressDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<LightingProgressDialog*>(lparam);
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




void LightingProgressDialog::OnWindowTimer()
{
    KillTimer(dialog_, 0);

    document_->CalculateLight(document_->GetLightList(),
        boost::bind(&LightingProgressDialog::OnTotal, this, _1),
        boost::bind(&LightingProgressDialog::OnStep, this, _1));

    EndDialog(dialog_, IDOK);
}

void LightingProgressDialog::OnTotal(int total)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS1);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, total));
    SendMessage(progress, PBM_SETPOS, 0, 0);
}

void LightingProgressDialog::OnStep(int value)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS1);
    SendMessage(progress, PBM_SETPOS, value, 0);
}
