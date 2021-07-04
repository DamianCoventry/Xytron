#include "choosetexturesetdialog.h"
#include "resource.h"
#include "dialogutil.h"

#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'ChooseTextureSetDialog *' of greater size

ChooseTextureSetDialog::ChooseTextureSetDialog()
: dialog_(NULL)
{
}

ChooseTextureSetDialog::~ChooseTextureSetDialog()
{
}

bool ChooseTextureSetDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CHOOSETEXTURESET_DIALOG), parent,
        ChooseTextureSetDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK ChooseTextureSetDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    ChooseTextureSetDialog* this_ = reinterpret_cast<ChooseTextureSetDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<ChooseTextureSetDialog*>(lparam);
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
        case IDC_TEXTURESETS:
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




void ChooseTextureSetDialog::OnInitDialog()
{
    HWND list = GetDlgItem(dialog_, IDC_TEXTURESETS);

    TextureSetDocumentMap::iterator i;
    for(i = ts_docs_->begin(); i != ts_docs_->end(); ++i)
    {
        ListBox_AddString(list, i->first.c_str());
    }
}

void ChooseTextureSetDialog::OnButtonOK()
{
    HWND list = GetDlgItem(dialog_, IDC_TEXTURESETS);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel == LB_ERR)
    {
        MessageBox(dialog_, "Please select a Texture Set", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_TEXTURESETS));
        return;
    }

    char buffer[MAX_PATH];
    ListBox_GetText(list, cur_sel, buffer);
    ts_ = buffer;

    EndDialog(dialog_, IDOK);
}
