#include "loadmapprogressdialog.h"
#include "resource.h"
#include "dialogutil.h"
#include <commctrl.h>
#include <windowsx.h>

#include <boost/bind.hpp>
#include "../sagedocuments/cellmapfile.h"

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'LoadMapProgressDialog *' of greater size

bool LoadMapProgressDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_LOADMAPPROGRESS_DIALOG), parent,
        LoadMapProgressDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK LoadMapProgressDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    LoadMapProgressDialog* this_ = reinterpret_cast<LoadMapProgressDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<LoadMapProgressDialog*>(lparam);
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




void LoadMapProgressDialog::OnWindowTimer()
{
    KillTimer(dialog_, 0);

    SetDlgItemText(dialog_, IDC_STATIC1, "Binding Textures");
    SetDlgItemText(dialog_, IDC_STATIC2, "Loading Cell Map");
    SetDlgItemText(dialog_, IDC_STATIC3, "Building BSP Trees");

    document_->BuildTextureSetMap(res_cxt_, ts_docs_, content_dir_);
    document_->InitialiseDefaultTextureSet(res_cxt_);

    CellMapFile file;
    if(!file.Load(path_, document_,
        boost::bind(&LoadMapProgressDialog::OnCellMapFileTotal, this, _1),
        boost::bind(&LoadMapProgressDialog::OnCellMapFileStep, this, _1)))
    {
        EndDialog(dialog_, IDCANCEL);
        return;
    }

    document_->BindCellsToTextures(boost::bind(&LoadMapProgressDialog::OnBindTexturesTotal, this, _1),
        boost::bind(&LoadMapProgressDialog::OnBindTexturesStep, this, _1));

    document_->BuildCombinedCellBspTrees(boost::bind(&LoadMapProgressDialog::OnBuildCombinedCellBspTreesTotal, this, _1),
        boost::bind(&LoadMapProgressDialog::OnBuildCombinedCellBspTreesStep, this, _1));

    EndDialog(dialog_, IDOK);
}

void LoadMapProgressDialog::OnBindTexturesTotal(int total)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS2);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, total));
    SendMessage(progress, PBM_SETPOS, 0, 0);
}

void LoadMapProgressDialog::OnBindTexturesStep(int value)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS2);
    SendMessage(progress, PBM_SETPOS, value, 0);
}

void LoadMapProgressDialog::OnCellMapFileTotal(int total)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS1);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, total));
    SendMessage(progress, PBM_SETPOS, 0, 0);
}

void LoadMapProgressDialog::OnCellMapFileStep(int value)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS1);
    SendMessage(progress, PBM_SETPOS, value, 0);
}

void LoadMapProgressDialog::OnBuildCombinedCellBspTreesTotal(int total)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS3);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, total));
    SendMessage(progress, PBM_SETPOS, 0, 0);
}

void LoadMapProgressDialog::OnBuildCombinedCellBspTreesStep(int value)
{
    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS3);
    SendMessage(progress, PBM_SETPOS, value, 0);
}
