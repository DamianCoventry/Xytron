#include "creatingthumbnailsdialog.h"
#include "resource.h"
#include "dialogutil.h"
#include "dibresources.h"

#include <sstream>

#include <commctrl.h>
#include <windowsx.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'CreatingThumbnailsDialog *' of greater size

bool CreatingThumbnailsDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CREATINGTHUMBNAILS), parent,
        CreatingThumbnailsDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}

std::vector<std::string> CreatingThumbnailsDialog::DirectoryListing(const std::string& filespec)
{
    std::vector<std::string> rv;

    std::ostringstream oss;
    oss << content_dir_ << "\\" << filespec;

    WIN32_FIND_DATA find_data;
    memset(&find_data, 0, sizeof(WIN32_FIND_DATA));

    HANDLE find_handle = FindFirstFile(oss.str().c_str(), &find_data);
    if(find_handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                rv.push_back(find_data.cFileName);
            }
        }
        while(FindNextFile(find_handle, &find_data));

        FindClose(find_handle);
    }

    return rv;
}




INT_PTR CALLBACK CreatingThumbnailsDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    CreatingThumbnailsDialog* this_ = reinterpret_cast<CreatingThumbnailsDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<CreatingThumbnailsDialog*>(lparam);
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




void CreatingThumbnailsDialog::OnInitDialog()
{
    SetTimer(dialog_, 0, 250, NULL);
}

void CreatingThumbnailsDialog::OnWindowTimer()
{
    KillTimer(dialog_, 0);

    // Create image thumbnails for each texture on disk
    std::vector<std::string> filenames;

    std::vector<std::string> temp(DirectoryListing("Textures\\*.bmp"));
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    temp = DirectoryListing("Textures\\*.tga");
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    temp = DirectoryListing("Textures\\*.png");
    std::copy(temp.begin(), temp.end(), std::back_inserter(filenames));

    HDC dc = GetDC(dialog_);

    HWND progress = GetDlgItem(dialog_, IDC_PROGRESS1);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, filenames.size()));
    SendMessage(progress, PBM_SETSTEP, 1, 0);

    std::vector<std::string>::iterator itor;
    for(itor = filenames.begin(); itor != filenames.end(); ++itor)
    {
        dib_resources_->Load(dc, content_dir_ + "\\Textures\\" + *itor);
        SendMessage(progress, PBM_STEPIT, 0, 0);
    }

    progress = GetDlgItem(dialog_, IDC_PROGRESS2);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, img_doc_map_->size()));
    SendMessage(progress, PBM_SETSTEP, 1, 0);

    // Create image thumbnails for each defined image
    ImageDocumentMap::iterator img_itor;
    for(img_itor = img_doc_map_->begin(); img_itor != img_doc_map_->end(); ++img_itor)
    {
        POINT position;
        position.x = img_itor->second.FrameX();
        position.y = img_itor->second.FrameY();

        SIZE size;
        size.cx = img_itor->second.FrameWidth();
        size.cy = img_itor->second.FrameHeight();

        dib_resources_->Create(
            dc, content_dir_ + "\\Textures\\" + img_itor->second.TextureFilename(),
            position, size, "Image:" + img_itor->first);

        SendMessage(progress, PBM_STEPIT, 0, 0);
    }

    progress = GetDlgItem(dialog_, IDC_PROGRESS3);
    SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, img_anim_doc_map_->size()));
    SendMessage(progress, PBM_SETSTEP, 1, 0);

    // Create image thumbnails for each defined image animation
    ImageAnimationDocumentMap::iterator img_anim_itor;
    for(img_anim_itor = img_anim_doc_map_->begin(); img_anim_itor != img_anim_doc_map_->end(); ++img_anim_itor)
    {
        POINT position;
        position.x = img_anim_itor->second.StartX();
        position.y = img_anim_itor->second.StartY();

        SIZE size;
        size.cx = img_anim_itor->second.FrameWidth();
        size.cy = img_anim_itor->second.FrameHeight();

        dib_resources_->Create(
            dc, content_dir_ + "\\Textures\\" + img_anim_itor->second.TextureFilename(),
            position, size, "ImageAnim:" + img_anim_itor->first);

        SendMessage(progress, PBM_STEPIT, 0, 0);
    }

    ReleaseDC(dialog_, dc);
    EndDialog(dialog_, IDOK);
}
