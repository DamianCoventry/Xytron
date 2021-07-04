#include "gamepropertiesdialog.h"
#include "resource.h"
#include "dialogutil.h"
#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'GamePropertiesDialog *' of greater size

GamePropertiesDialog::GamePropertiesDialog()
: dialog_(NULL)
, clear_back_buffer_(false)
, sm_docs_(NULL)
{
}

bool GamePropertiesDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_GAME_PROPERTIES_DIALOG), parent,
        GamePropertiesDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK GamePropertiesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    GamePropertiesDialog* this_ = reinterpret_cast<GamePropertiesDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<GamePropertiesDialog*>(lparam);
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




void GamePropertiesDialog::OnInitDialog()
{
    if(!name_.empty())
    {
        SetDlgItemText(dialog_, IDC_NAME, name_.c_str());
    }

    HWND combo = GetDlgItem(dialog_, IDC_STATEMACHINE);
    StateMachineDocumentMap::iterator itor;
    for(itor = sm_docs_->begin(); itor != sm_docs_->end(); ++itor)
    {
        int index = ComboBox_AddString(combo, itor->first.c_str());
        if(state_machine_ == itor->first)
        {
            ComboBox_SetCurSel(combo, index);
        }
    }

    CheckDlgButton(dialog_, IDC_CLEARBACKBUFFER, clear_back_buffer_ ? BST_CHECKED : BST_UNCHECKED);
}

void GamePropertiesDialog::OnButtonOK()
{
    char buffer[128];
    if(GetDlgItemText(dialog_, IDC_NAME, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please a name for this game", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_NAME));
        return;
    }
    name_ = buffer;

    HWND combo = GetDlgItem(dialog_, IDC_STATEMACHINE);
    int cur_sel = ComboBox_GetCurSel(combo);
    if(cur_sel == CB_ERR)
    {
        MessageBox(dialog_, "Please select a state machine", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_STATEMACHINE));
        return;
    }
    ComboBox_GetText(combo, buffer, 128);
    state_machine_ = buffer;

    clear_back_buffer_ = (IsDlgButtonChecked(dialog_, IDC_CLEARBACKBUFFER) == BST_CHECKED);

    EndDialog(dialog_, IDOK);
}
