#include "savefiledialog.h"
#include <commdlg.h>

std::string SaveFileDialog::Get(HINSTANCE instance, HWND parent, const char* filter, const char* default_ext, const std::string& initial_filename)
{
    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));

    char filename[MAX_PATH];
    memset(filename, 0, sizeof(char)*MAX_PATH);
    if(!initial_filename.empty())
    {
        strcpy_s(filename, MAX_PATH, initial_filename.c_str());
    }

    ofn.lStructSize     = sizeof(OPENFILENAME);
    ofn.hwndOwner       = parent;
    ofn.hInstance       = instance;
    ofn.lpstrFilter     = filter;
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = MAX_PATH;
    ofn.Flags           = OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    ofn.lpstrDefExt     = default_ext;

    if(GetSaveFileName(&ofn))
    {
        return std::string(filename);
    }
    return std::string();
}
