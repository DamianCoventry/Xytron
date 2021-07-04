#include "defaultbindingsdialogbar.h"
#include "resource.h"

#include <stdexcept>
#include <sstream>

#include <commctrl.h>
#include <windowsx.h>

#include "choosebindcommanddialog.h"
#include "../util/string.h"
#include <boost/algorithm/string.hpp>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'DefaultBindingsDialogBar *' of greater size

using namespace boost::algorithm;

DefaultBindingsDialogBar::DefaultBindingsDialogBar(HINSTANCE instance, HWND parent, DefaultBindingsDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_DEFAULTBINDINGS_DIALOGBAR), parent,
        DefaultBindingsDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an DefaultBindingsDialogBar instance");
    }

    HWND lv = GetDlgItem(handle_, IDC_BINDINGS);
    ListView_SetExtendedListViewStyle(lv, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    // Insert the two columns
    LVCOLUMN col;
    memset(&col, 0, sizeof(LVCOLUMN));

    char buffer[128];
    memset(buffer, 0, sizeof(char)*128);

    col.cx      = 300;
    col.mask    = LVCF_TEXT | LVCF_WIDTH;
    col.pszText = buffer;

    strcpy_s(buffer, "Input Event");
    ListView_InsertColumn(lv, 0, &col);

    strcpy_s(buffer, "Game Command");
    ListView_InsertColumn(lv, 1, &col);


    // Insert all the items.  In this case the items are our input events.  Only supporting
    // the mouse and keyboard at this stage.  Joystick support could be added... I'd have to
    // purchase a Joystick first though ;-)
    LVITEM item;
    memset(&item, 0, sizeof(LVITEM));

    item.mask       = LVIF_TEXT;
    item.pszText    = buffer;

    strcpy_s(buffer, "KEY_ESCAPE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_1");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_2");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_3");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_4");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_5");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_6");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_7");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_8");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_9");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_0");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_MINUS");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_EQUALS");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_BACK");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_TAB");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_Q");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_W");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_E");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_R");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_T");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_Y");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_U");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_I");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_O");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_P");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_LBRACKET");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_RBRACKET");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_RETURN");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_LCONTROL");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_A");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_S");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_D");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_G");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_H");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_J");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_K");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_L");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_SEMICOLON");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_APOSTROPHE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_GRAVE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_LSHIFT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_BACKSLASH");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_Z");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_X");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_C");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_V");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_B");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_N");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_M");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_COMMA");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_PERIOD");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_SLASH");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_RSHIFT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_MULTIPLY");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_LMENU");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_SPACE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_CAPITAL");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F1");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F2");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F3");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F4");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F5");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F6");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F7");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F8");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F9");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F10");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMLOCK");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_SCROLL");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD7");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD8");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD9");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_SUBTRACT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD4");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD5");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD6");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_ADD");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD1");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD2");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD3");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPAD0");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_DECIMAL");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_OEM_102");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F11");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F12");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F13");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F14");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_F15");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_KANA");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_ABNT_C1");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_CONVERT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NOCONVERT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_YEN");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_ABNT_C2");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPADEQUALS");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_PREVTRACK");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_AT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_COLON");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_UNDERLINE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_KANJI");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_STOP");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_AX");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_UNLABELED");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NEXTTRACK");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPADENTER");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_RCONTROL");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_MUTE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_CALCULATOR");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_PLAYPAUSE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_MEDIASTOP");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_VOLUMEDOWN");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_VOLUMEUP");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_WEBHOME");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NUMPADCOMMA");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_DIVIDE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_SYSRQ");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_RMENU");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_PAUSE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_HOME");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_UP");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_PRIOR");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_LEFT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_RIGHT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_END");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_DOWN");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_NEXT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_INSERT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_DELETE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_LWIN");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_RWIN");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_APPS");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_POWER");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_SLEEP");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_WAKE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_WEBSEARCH");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_WEBFAVORITES");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_WEBREFRESH");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_WEBSTOP");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_WEBFORWARD");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_WEBBACK");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_MYCOMPUTER");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_MAIL");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "KEY_MEDIASELECT");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "MSE_MOVE");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "MSE_MOVEX");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "MSE_MOVEY");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "MSE_BUTTON0");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "MSE_BUTTON1");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "MSE_BUTTON2");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "MSE_MWHEELUP");
    ListView_InsertItem(lv, &item);

    strcpy_s(buffer, "MSE_MWHEELDOWN");
    ListView_InsertItem(lv, &item);
}

void DefaultBindingsDialogBar::BindingsDocument(BindingsFile* docs)
{
    bind_docs_ = docs;

    HWND lv = GetDlgItem(handle_, IDC_BINDINGS);
    // The first thing we do is clear all the bindings from the
    // list view control.  We don't call OnButtonClearAll() because
    // that calls the event handler, which we don't want to do.
    char buffer[128];
    memset(buffer, 0, sizeof(char)*128);

    int count = ListView_GetItemCount(lv);
    for(int i = 0; i < count; i++)
    {
        ListView_SetItemText(lv, i, 1, buffer);
    }

    // Now insert all the given bindings into the list view.
    LVFINDINFO info;
    memset(&info, 0, sizeof(LVFINDINFO));
    info.flags  = LVFI_STRING;
    info.psz    = buffer;;

    BindingsFile::Iterator i;
    for(i = bind_docs_->Begin(); i != bind_docs_->End(); ++i)
    {
        strcpy_s(buffer, 128, i->first.c_str());
        int index = ListView_FindItem(lv, -1, &info);
        if(index >= 0)
        {
            strcpy_s(buffer, 128, i->second.c_str());
            ListView_SetItemText(lv, index, 1, buffer);
        }
    }
}

DefaultBindingsDialogBar::~DefaultBindingsDialogBar()
{
    DestroyWindow(handle_);
}

void DefaultBindingsDialogBar::RepositionWithinParent()
{
    RECT rect;
    GetClientRect(GetParent(handle_), &rect);
    SetWindowPos(handle_, NULL, 200, 0, rect.right-200, rect.bottom, SWP_NOZORDER);

    GetClientRect(handle_, &rect);

    RECT list_box_rect;
    GetWindowRect(GetDlgItem(handle_, IDC_FILES), &list_box_rect);
    ScreenToClient(handle_, (POINT*)&list_box_rect.left);

    SetWindowPos(
        GetDlgItem(handle_, IDC_BINDINGS), NULL, 0, 0,
        rect.right - 116, rect.bottom - 16,
        SWP_NOMOVE | SWP_NOZORDER);

    SetWindowPos(GetDlgItem(handle_, IDC_SET), NULL, rect.right-100, 8, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_CLEAR), NULL, rect.right-100, 38, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(GetDlgItem(handle_, IDC_CLEARALL), NULL, rect.right-100, 68, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void DefaultBindingsDialogBar::Enable()
{
    EnableWindow(handle_, TRUE);
}

void DefaultBindingsDialogBar::Disable()
{
    EnableWindow(handle_, FALSE);
}

void DefaultBindingsDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
}

void DefaultBindingsDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

void DefaultBindingsDialogBar::Clear()
{
    OnButtonClearAll();
}





INT_PTR CALLBACK DefaultBindingsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    DefaultBindingsDialogBar* this_ = reinterpret_cast<DefaultBindingsDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<DefaultBindingsDialogBar*>(lparam);
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_SET:
            this_->OnButtonSet();
            break;
        case IDC_CLEAR:
            this_->OnButtonClear();
            break;
        case IDC_CLEARALL:
            this_->OnButtonClearAll();
            break;
        }
        break;
    case WM_NOTIFY:
        {
            NMHDR* header_info = reinterpret_cast<NMHDR*>(lparam);
            switch(header_info->code)
            {
            case NM_DBLCLK:
                {
                    NMITEMACTIVATE* info = reinterpret_cast<NMITEMACTIVATE*>(lparam);
                    this_->OnDoubleClickBindingsListView(info->iItem, info->iSubItem);
                    break;
                }
            }
            break;
        }
    }

    return FALSE;
}





void DefaultBindingsDialogBar::OnButtonSet()
{
    HWND lv = GetDlgItem(handle_, IDC_BINDINGS);

    int index = ListView_GetNextItem(lv, -1, LVNI_SELECTED);
    if(index == -1)
    {
        MessageBox(handle_, "Please select an item first", "Information", MB_OK | MB_ICONINFORMATION);
        return;
    }

    char buffer[128];
    memset(buffer, 0, sizeof(char)*128);
    ListView_GetItemText(lv, index, 1, buffer, 128);

    std::vector<std::string> cl;
    if(!std::string(buffer).empty())
    {
        std::vector<std::string>::iterator i;
        for(i = cl.begin(); i != cl.end(); ++i)
        {
            trim_if(*i, is_any_of("\""));
        }
        cl = Util::Split(buffer);
    }

    ChooseBindCommandDialog dialog;
    dialog.CameraDocuments(cam_docs_);
    dialog.CommandLine(cl);
    if(dialog.Run(instance_, handle_))
    {
        const std::vector<std::string>& cl = dialog.CommandLine();

        std::ostringstream oss;
        oss << cl[0];

        std::vector<std::string>::const_iterator i = cl.begin();
        ++i;

        for( ; i != cl.end(); ++i)
        {
            oss << " \"" << *i << "\"";
        }

        strcpy_s(buffer, 128, oss.str().c_str());
        ListView_SetItemText(lv, index, 1, buffer);

        ListView_GetItemText(lv, index, 0, buffer, 128);
        event_handler_->OnDefaultBindingsChanged(buffer, oss.str());
    }
}

void DefaultBindingsDialogBar::OnButtonClear()
{
    HWND lv = GetDlgItem(handle_, IDC_BINDINGS);

    int index = ListView_GetNextItem(lv, -1, LVNI_SELECTED);
    if(index == -1)
    {
        MessageBox(handle_, "Please select an item first", "Information", MB_OK | MB_ICONINFORMATION);
        return;
    }

    char buffer[128];
    ListView_GetItemText(lv, index, 1, buffer, 128);
    event_handler_->OnDefaultBindingsRemoved(buffer);

    memset(buffer, 0, sizeof(char)*128);
    ListView_SetItemText(lv, index, 1, buffer);
}

void DefaultBindingsDialogBar::OnButtonClearAll()
{
    char buffer[128];
    memset(buffer, 0, sizeof(char)*128);

    HWND lv = GetDlgItem(handle_, IDC_BINDINGS);
    int count = ListView_GetItemCount(lv);
    for(int i = 0; i < count; i++)
    {
        ListView_SetItemText(lv, i, 1, buffer);
    }

    event_handler_->OnAllDefaultBindingsCleared();
}

void DefaultBindingsDialogBar::OnDoubleClickBindingsListView(int item_index, int sub_item_index)
{
    OnButtonSet();
}
