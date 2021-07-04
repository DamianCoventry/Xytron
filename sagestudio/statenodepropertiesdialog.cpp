#include "statenodepropertiesdialog.h"
#include "resource.h"
#include "dialogutil.h"
#include "commanddialog.h"
#include "../sagedocuments/statemachinedocument.h"

#include <commctrl.h>
#include <windowsx.h>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'StateNodePropertiesDialog *' of greater size

StateNodePropertiesDialog::StateNodePropertiesDialog()
: image_anim_map_(NULL)
, image_map_(NULL)
{
}

bool StateNodePropertiesDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_STATE_NODE_PROPERTIES), parent,
        StateNodePropertiesDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK StateNodePropertiesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    StateNodePropertiesDialog* this_ = reinterpret_cast<StateNodePropertiesDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<StateNodePropertiesDialog*>(lparam);
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
        case IDC_INSERT:
            this_->OnButtonInsert();
            break;
        case IDC_EDIT:
            this_->OnButtonEdit();
            break;
        case IDC_REMOVE:
            this_->OnButtonRemove();
            break;
        case IDC_CLEARALL:
            this_->OnButtonClearAll();
            break;
        case IDOK:
            this_->OnButtonOK();
            break;
        case IDCANCEL:
            EndDialog(dialog, IDCANCEL);
            break;
        case IDC_MOVEUP:
            this_->OnButtonMoveUp();
            break;
        case IDC_MOVEDOWN:
            this_->OnButtonMoveDown();
            break;
        case IDC_EVENTS:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboEventsChanged();
            }
            break;
        case IDC_COMMANDS:
            if(HIWORD(wparam) == LBN_DBLCLK)
            {
                this_->OnButtonEdit();
            }
            break;
        }
        break;
    }

    return FALSE;
}




void StateNodePropertiesDialog::OnInitDialog()
{
    if(!name_.empty())
    {
        SetDlgItemText(dialog_, IDC_NAME, name_.c_str());
    }

    HWND combo = GetDlgItem(dialog_, IDC_EVENTS);
    ComboBox_AddString(combo, "OnBegin");
    ComboBox_AddString(combo, "OnEnd");
    ComboBox_AddString(combo, "OnThink");
    ComboBox_AddString(combo, "OnMove");
    ComboBox_AddString(combo, "OnDraw");
    ComboBox_AddString(combo, "OnTimer");
    ComboBox_SetCurSel(combo, 0);

    OnComboEventsChanged();
}

void StateNodePropertiesDialog::OnButtonOK()
{
    char buffer[128];
    if(GetDlgItemText(dialog_, IDC_NAME, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter a name for this state", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_NAME));
        return;
    }
    name_ = buffer;

    EndDialog(dialog_, IDOK);
}

void StateNodePropertiesDialog::OnButtonInsert()
{
    CommandDialog dialog;
    dialog.SetImageAnimationDocumentMap(image_anim_map_);
    dialog.SetImageDocumentMap(image_map_);
    dialog.DibResources(dib_resources_);
    dialog.ContentDirectory(content_dir_);
    dialog.SetResourceContextDocumentMap(res_ctxt_map_);
    dialog.SetCellMapDocumentList(cell_map_list_);
    dialog.SetCameraDocuments(cam_map_);
    if(dialog.Run(GetWindowInstance(dialog_), dialog_))
    {
        std::ostringstream oss;
        oss << dialog.Command();
        std::vector<std::string>::const_iterator itor;
        for(itor = dialog.Parameters().begin(); itor != dialog.Parameters().end(); ++itor)
        {
            oss << " \"" << *itor << "\"";
        }

        HWND list = GetDlgItem(dialog_, IDC_COMMANDS);
        ListBox_AddString(list, oss.str().c_str());

        char buffer[128];
        GetDlgItemText(dialog_, IDC_EVENTS, buffer, 128);

        // Insert this into state_doc_
        DocCommandLineSets& ecls = state_doc_->EventCommandLineSets();
        DocCommandLineSets::iterator ecls_itor = ecls.find(buffer);
        if(ecls_itor != ecls.end())
        {
            DocCommandLine cl;
            cl.push_back(dialog.Command());
            std::copy(dialog.Parameters().begin(),
                      dialog.Parameters().end(),
                      std::back_inserter(cl));
            ecls_itor->second.push_back(cl);
        }
    }
}

void StateNodePropertiesDialog::OnButtonEdit()
{
    HWND list = GetDlgItem(dialog_, IDC_COMMANDS);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel != LB_ERR)
    {
        char buffer[128];
        GetDlgItemText(dialog_, IDC_EVENTS, buffer, 128);

        DocCommandLineSets& ecls = state_doc_->EventCommandLineSets();
        DocCommandLineSets::iterator ecls_itor = ecls.find(buffer);
        if(ecls_itor == ecls.end())
        {
            return;
        }

        DocCommandLineSet& cls = ecls_itor->second;
        DocCommandLineSet::iterator cls_itor = cls.begin();
        std::advance(cls_itor, cur_sel);

        CommandDialog dialog;
        dialog.SetImageAnimationDocumentMap(image_anim_map_);
        dialog.SetImageDocumentMap(image_map_);
        dialog.DibResources(dib_resources_);
        dialog.ContentDirectory(content_dir_);
        dialog.SetResourceContextDocumentMap(res_ctxt_map_);
        dialog.SetCellMapDocumentList(cell_map_list_);
        dialog.SetCameraDocuments(cam_map_);
        dialog.Command(cls_itor->front());

        std::vector<std::string> temp;
        std::copy(cls_itor->begin()+1, cls_itor->end(), std::back_inserter(temp));
        dialog.Parameters(temp);

        if(dialog.Run(GetWindowInstance(dialog_), dialog_))
        {
            std::ostringstream oss;
            oss << dialog.Command();
            std::vector<std::string>::const_iterator itor;
            for(itor = dialog.Parameters().begin(); itor != dialog.Parameters().end(); ++itor)
            {
                oss << " \"" << *itor << "\"";
            }

            HWND list = GetDlgItem(dialog_, IDC_COMMANDS);
            ListBox_DeleteString(list, cur_sel);
            ListBox_InsertString(list, cur_sel, oss.str().c_str());
            ListBox_SetCurSel(list, cur_sel);

            // Update state_doc_
            char buffer[128];
            GetDlgItemText(dialog_, IDC_EVENTS, buffer, 128);

            DocCommandLineSets& ecls = state_doc_->EventCommandLineSets();
            DocCommandLineSets::iterator ecls_itor = ecls.find(buffer);
            if(ecls_itor != ecls.end())
            {
                DocCommandLine cl;
                cl.push_back(dialog.Command());
                std::copy(dialog.Parameters().begin(),
                          dialog.Parameters().end(),
                          std::back_inserter(cl));

                DocCommandLineSet& cls = ecls_itor->second;
                DocCommandLineSet::iterator cls_itor = cls.begin();
                std::advance(cls_itor, cur_sel);
                *cls_itor = cl;
            }
        }
    }
}

void StateNodePropertiesDialog::OnButtonRemove()
{
    HWND list = GetDlgItem(dialog_, IDC_COMMANDS);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel != LB_ERR)
    {
        ListBox_DeleteString(list, cur_sel);

        // Update state_doc_
        char buffer[128];
        GetDlgItemText(dialog_, IDC_EVENTS, buffer, 128);

        DocCommandLineSets& ecls = state_doc_->EventCommandLineSets();
        DocCommandLineSets::iterator ecls_itor = ecls.find(buffer);
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor = cls.begin();
            std::advance(cls_itor, cur_sel);
            cls.erase(cls_itor);
        }
    }
}

void StateNodePropertiesDialog::OnButtonClearAll()
{
    HWND list = GetDlgItem(dialog_, IDC_COMMANDS);
    ListBox_ResetContent(list);

    // Update state_doc_
    char buffer[128];
    GetDlgItemText(dialog_, IDC_EVENTS, buffer, 128);

    DocCommandLineSets& ecls = state_doc_->EventCommandLineSets();
    DocCommandLineSets::iterator ecls_itor = ecls.find(buffer);
    if(ecls_itor != ecls.end())
    {
        ecls_itor->second.clear();
    }
}

void StateNodePropertiesDialog::OnComboEventsChanged()
{
    HWND list = GetDlgItem(dialog_, IDC_COMMANDS);
    ListBox_ResetContent(list);

    char buffer[128];
    GetDlgItemText(dialog_, IDC_EVENTS, buffer, 128);

    DocCommandLineSets& ecls = state_doc_->EventCommandLineSets();
    DocCommandLineSets::iterator ecls_itor = ecls.find(buffer);
    if(ecls_itor != ecls.end())
    {
        DocCommandLineSet::iterator cls_itor;
        for(cls_itor = ecls_itor->second.begin(); cls_itor != ecls_itor->second.end(); ++cls_itor)
        {
            DocCommandLine& cl = *cls_itor;
            if(!cl.empty())
            {
                DocCommandLine::iterator arg = cl.begin();
                std::ostringstream oss;
                oss << *arg;
                ++arg;

                for( ; arg != cl.end(); ++arg)
                {
                    oss << " \"" << *arg << "\"";
                }

                ListBox_AddString(list, oss.str().c_str());
            }
        }
    }
}

void StateNodePropertiesDialog::OnButtonMoveUp()
{
    HWND list = GetDlgItem(dialog_, IDC_COMMANDS);
    int cur_sel = ListBox_GetCurSel(list);
    if(cur_sel != LB_ERR && cur_sel > 0)
    {
        char buffer[128];
        ListBox_GetText(list, cur_sel, buffer);
        ListBox_DeleteString(list, cur_sel);
        ListBox_InsertString(list, cur_sel-1, buffer);
        ListBox_SetCurSel(list, cur_sel-1);

        // Update state_doc_
        GetDlgItemText(dialog_, IDC_EVENTS, buffer, 128);

        DocCommandLineSets& ecls = state_doc_->EventCommandLineSets();
        DocCommandLineSets::iterator ecls_itor = ecls.find(buffer);
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor = cls.begin();
            std::advance(cls_itor, cur_sel);

            DocCommandLine copy = *cls_itor;
            cls.erase(cls_itor);

            cls_itor = cls.begin();
            std::advance(cls_itor, cur_sel-1);
            cls.insert(cls_itor, copy);
        }
    }
}

void StateNodePropertiesDialog::OnButtonMoveDown()
{
    HWND list = GetDlgItem(dialog_, IDC_COMMANDS);
    int cur_sel = ListBox_GetCurSel(list);
    int count = ListBox_GetCount(list);
    if(cur_sel != LB_ERR && cur_sel < count-1)
    {
        char buffer[128];
        ListBox_GetText(list, cur_sel, buffer);
        ListBox_DeleteString(list, cur_sel);
        ListBox_InsertString(list, cur_sel+1, buffer);
        ListBox_SetCurSel(list, cur_sel+1);

        // Update state_doc_
        GetDlgItemText(dialog_, IDC_EVENTS, buffer, 128);

        DocCommandLineSets& ecls = state_doc_->EventCommandLineSets();
        DocCommandLineSets::iterator ecls_itor = ecls.find(buffer);
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor = cls.begin();
            std::advance(cls_itor, cur_sel);

            DocCommandLine copy = *cls_itor;
            cls.erase(cls_itor);

            cls_itor = cls.begin();
            std::advance(cls_itor, cur_sel+1);
            cls.insert(cls_itor, copy);
        }
    }
}
