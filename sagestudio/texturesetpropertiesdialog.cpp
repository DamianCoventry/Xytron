#include "texturesetpropertiesdialog.h"
#include "resource.h"
#include "dialogutil.h"
#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'TextureSetPropertiesDialog *' of greater size

bool TextureSetPropertiesDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_TEXTURESETPROPERTIES_DIALOG), parent,
        TextureSetPropertiesDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK TextureSetPropertiesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    TextureSetPropertiesDialog* this_ = reinterpret_cast<TextureSetPropertiesDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<TextureSetPropertiesDialog*>(lparam);
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




void TextureSetPropertiesDialog::OnInitDialog()
{
    if(!name_.empty())
    {
        SetDlgItemText(dialog_, IDC_NAME, name_.c_str());
    }

    HWND combo = GetDlgItem(dialog_, IDC_RESCXT);
    MyTest::ResourceContextDocumentMap::iterator i;
    for(i = rc_map_->begin(); i != rc_map_->end(); ++i)
    {
        ComboBox_AddString(combo, i->first.c_str());
    }

    if(res_cxt_.empty())
    {
        ComboBox_SetCurSel(combo, 0);
    }
    else
    {
        ComboBox_SelectString(combo, -1, res_cxt_.c_str());
    }
}

void TextureSetPropertiesDialog::OnButtonOK()
{
    char buffer[128];
    if(GetDlgItemText(dialog_, IDC_NAME, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter a name for this texture set", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_NAME));
        return;
    }
    name_ = buffer;

    HWND combo = GetDlgItem(dialog_, IDC_RESCXT);
    int cur_sel = ComboBox_GetCurSel(combo);
    if(cur_sel == CB_ERR)
    {
        MessageBox(dialog_, "Please select a resource context", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_RESCXT));
        return;
    }
    ComboBox_GetText(combo, buffer, 128);
    res_cxt_ = buffer;

    EndDialog(dialog_, IDOK);
}
