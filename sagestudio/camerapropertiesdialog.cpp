#include "camerapropertiesdialog.h"
#include "resource.h"
#include "dialogutil.h"

#include <boost/lexical_cast.hpp>

#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'CameraPropertiesDialog *' of greater size

bool CameraPropertiesDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CAMERA_PROPERTIES), parent,
        CameraPropertiesDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK CameraPropertiesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    CameraPropertiesDialog* this_ = reinterpret_cast<CameraPropertiesDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<CameraPropertiesDialog*>(lparam);
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




void CameraPropertiesDialog::OnInitDialog()
{
    if(!name_.empty())
    {
        SetDlgItemText(dialog_, IDC_NAME, name_.c_str());
    }

    std::string temp = boost::lexical_cast<std::string>(position_.x_);
    SetDlgItemText(dialog_, IDC_POS_X, temp.c_str());

    temp = boost::lexical_cast<std::string>(position_.y_);
    SetDlgItemText(dialog_, IDC_POS_Y, temp.c_str());

    temp = boost::lexical_cast<std::string>(position_.z_);
    SetDlgItemText(dialog_, IDC_POS_Z, temp.c_str());

    temp = boost::lexical_cast<std::string>(rotation_.x_);
    SetDlgItemText(dialog_, IDC_ROT_X, temp.c_str());

    temp = boost::lexical_cast<std::string>(rotation_.y_);
    SetDlgItemText(dialog_, IDC_ROT_Y, temp.c_str());

    temp = boost::lexical_cast<std::string>(rotation_.z_);
    SetDlgItemText(dialog_, IDC_ROT_Z, temp.c_str());
}

void CameraPropertiesDialog::OnButtonOK()
{
    char buffer[128];
    if(GetDlgItemText(dialog_, IDC_NAME, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter a name", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_NAME));
        return;
    }
    name_ = buffer;



    if(GetDlgItemText(dialog_, IDC_POS_X, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter an X position", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_POS_X));
        return;
    }
    try {
        position_.x_ = boost::lexical_cast<float>(buffer);
    }
    catch(boost::bad_lexical_cast& ) { }

    if(GetDlgItemText(dialog_, IDC_POS_Y, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter a Y position", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_POS_Y));
        return;
    }
    try {
        position_.y_ = boost::lexical_cast<float>(buffer);
    }
    catch(boost::bad_lexical_cast& ) { }

    if(GetDlgItemText(dialog_, IDC_POS_Z, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter a Z position", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_POS_Z));
        return;
    }
    try {
        position_.z_ = boost::lexical_cast<float>(buffer);
    }
    catch(boost::bad_lexical_cast& ) { }



    if(GetDlgItemText(dialog_, IDC_ROT_X, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter an X rotation", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_ROT_X));
        return;
    }
    try {
        rotation_.x_ = boost::lexical_cast<float>(buffer);
    }
    catch(boost::bad_lexical_cast& ) { }

    if(GetDlgItemText(dialog_, IDC_ROT_Y, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter a Y rotation", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_ROT_Y));
        return;
    }
    try {
        rotation_.y_ = boost::lexical_cast<float>(buffer);
    }
    catch(boost::bad_lexical_cast& ) { }

    if(GetDlgItemText(dialog_, IDC_ROT_Z, buffer, 128) <= 0)
    {
        MessageBox(dialog_, "Please enter a Z rotation", "Information", MB_OK | MB_ICONINFORMATION);
        SetFocus(GetDlgItem(dialog_, IDC_ROT_Z));
        return;
    }
    try {
        rotation_.z_ = boost::lexical_cast<float>(buffer);
    }
    catch(boost::bad_lexical_cast& ) { }

    EndDialog(dialog_, IDOK);
}
