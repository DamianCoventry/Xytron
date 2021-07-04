#include "statemachinetoolsdialogbar.h"
#include "resource.h"
#include <stdexcept>
#include <commctrl.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'StateMachineToolsDialogBar *' of greater size

StateMachineToolsDialogBar::StateMachineToolsDialogBar(HINSTANCE instance, HWND parent, StateMachineToolsDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_STATEMACHINE_DIALOGBAR), parent,
        StateMachineToolsDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an StateMachineToolsDialogBar instance");
    }
}

StateMachineToolsDialogBar::~StateMachineToolsDialogBar()
{
    DestroyWindow(handle_);
}

void StateMachineToolsDialogBar::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, parent_client_rect.right-150, 0, 150, parent_client_rect.bottom, SWP_NOZORDER);
}

void StateMachineToolsDialogBar::Enable()
{
    EnableWindow(GetDlgItem(handle_, IDC_SELECT), TRUE);
    EnableWindow(GetDlgItem(handle_, IDC_INSERT), TRUE);
    EnableWindow(GetDlgItem(handle_, IDC_REMOVE), TRUE);
    EnableWindow(GetDlgItem(handle_, IDC_PROPERTIES), TRUE);
    EnableWindow(GetDlgItem(handle_, IDC_CLEARALL), TRUE);
    EnableWindow(handle_, TRUE);
}

void StateMachineToolsDialogBar::Disable()
{
    EnableWindow(GetDlgItem(handle_, IDC_SELECT), FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_INSERT), FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_REMOVE), FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_PROPERTIES), FALSE);
    EnableWindow(GetDlgItem(handle_, IDC_CLEARALL), FALSE);
    EnableWindow(handle_, FALSE);
}

void StateMachineToolsDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void StateMachineToolsDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}






INT_PTR CALLBACK StateMachineToolsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    StateMachineToolsDialogBar* this_ = reinterpret_cast<StateMachineToolsDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<StateMachineToolsDialogBar*>(lparam);
            this_->handle_ = dialog;
            this_->OnInitDialog();
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_SELECT:
            this_->event_handler_->OnStateMachineSelect();
            break;
        case IDC_INSERT:
            this_->OnButtonInsert();
            break;
        case ID_INSERT_ENTRYNODE:
            this_->event_handler_->OnStateMachineInsertEntry();
            break;
        case ID_INSERT_EXITNODE:
            this_->event_handler_->OnStateMachineInsertExit();
            break;
        case ID_INSERT_STATENODE:
            this_->event_handler_->OnStateMachineInsertState();
            break;
        case ID_INSERT_EVENT:
            this_->event_handler_->OnStateMachineInsertEvent();
            break;
        case IDC_REMOVE:
            this_->event_handler_->OnStateMachineDelete();
            break;
        case IDC_PROPERTIES:
            this_->event_handler_->OnStateMachineProperties();
            break;
        case IDC_CLEARALL:
            this_->OnButtonClearAll();
            break;
        }
        break;
    }

    return FALSE;
}

void StateMachineToolsDialogBar::OnInitDialog()
{
}

void StateMachineToolsDialogBar::OnButtonInsert()
{
    HMENU menu = LoadMenu(instance_, MAKEINTRESOURCE(IDR_STATE_MACHINE_MENU));
    HMENU sub_menu = GetSubMenu(menu, 0);

    RECT rect;
    GetWindowRect(GetDlgItem(handle_, IDC_INSERT), &rect);

    TrackPopupMenu(sub_menu, TPM_LEFTALIGN | TPM_TOPALIGN, rect.left, rect.bottom, 0, handle_, NULL);
}

void StateMachineToolsDialogBar::OnButtonClearAll()
{
    if(MessageBox(handle_, "Are you sure you want to completely clear all the content of this state machine?", "Confirmation", MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        event_handler_->OnStateMachineClearAll();
    }
}
