#include "choosedirectorydialog.h"
#include <shlobj.h>

std::string ChooseDirectoryDialog::Run(HWND parent, const std::string& caption)
{
    std::string rv;

    BROWSEINFO bi;
    memset(&bi, 0, sizeof(BROWSEINFO));

    char display_name[MAX_PATH];
    memset(display_name, 0, sizeof(char)*MAX_PATH);

    char caption_buffer[1024];
    strcpy_s(caption_buffer, 1024, caption.c_str());

    bi.hwndOwner        = parent;
    bi.pszDisplayName   = display_name;
    bi.lpszTitle        = caption_buffer;
    bi.ulFlags          = BIF_EDITBOX | BIF_NEWDIALOGSTYLE;

    ITEMIDLIST* id_list = SHBrowseForFolder(&bi);
    if(id_list)
    {
        char buffer[MAX_PATH];
        SHGetPathFromIDList(id_list, buffer);

        rv = buffer;

        IMalloc* malloc;
        SHGetMalloc(&malloc);
        malloc->Free(id_list);
    }

    return rv;
}
