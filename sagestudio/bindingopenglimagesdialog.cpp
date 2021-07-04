#include "bindingopenglimagesdialog.h"
#include "resource.h"
#include "dialogutil.h"
#include "../util/resourcecontext.h"

#include <sstream>

#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'BindingOpenGLImagesDialog *' of greater size

bool BindingOpenGLImagesDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_BINDINGOPENGLIMAGES_DIALOG), parent,
        BindingOpenGLImagesDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}




INT_PTR CALLBACK BindingOpenGLImagesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    BindingOpenGLImagesDialog* this_ = reinterpret_cast<BindingOpenGLImagesDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<BindingOpenGLImagesDialog*>(lparam);
            this_->dialog_ = dialog;
            this_->OnInitDialog();

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




void BindingOpenGLImagesDialog::OnInitDialog()
{
    SetTimer(dialog_, 0, 250, NULL);
}

void BindingOpenGLImagesDialog::OnWindowTimer()
{
    KillTimer(dialog_, 0);

    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, ts_docs_->size()));
    SendMessage(progress, PBM_SETSTEP, 1, 0);

    TextureSetDocumentMap::iterator i;
    for(i = ts_docs_->begin(); i != ts_docs_->end(); ++i)
    {
        if(!i->second.TexWall().empty())
        {
            res_cxt_->LoadTexture(content_dir_ + "\\Textures\\" + i->second.TexWall());
        }
        if(!i->second.TexCeiling().empty())
        {
            res_cxt_->LoadTexture(content_dir_ + "\\Textures\\" + i->second.TexCeiling());
        }
        if(!i->second.TexFloor().empty())
        {
            res_cxt_->LoadTexture(content_dir_ + "\\Textures\\" + i->second.TexFloor());
        }
        if(!i->second.TexLiquid().empty())
        {
            res_cxt_->LoadTexture(content_dir_ + "\\Textures\\" + i->second.TexLiquid());
        }
        if(!i->second.TexDoor().empty())
        {
            res_cxt_->LoadTexture(content_dir_ + "\\Textures\\" + i->second.TexDoor());
        }
        if(!i->second.TexTransWall().empty())
        {
            res_cxt_->LoadTexture(content_dir_ + "\\Textures\\" + i->second.TexTransWall());
        }
        if(!i->second.TexTransDoor().empty())
        {
            res_cxt_->LoadTexture(content_dir_ + "\\Textures\\" + i->second.TexTransDoor());
        }
        SendMessage(progress, PBM_STEPIT, 0, 0);
    }

    EndDialog(dialog_, IDOK);
}
