#include "openfiledialog.h"
#include <commdlg.h>
#include <boost/scoped_array.hpp>

std::string OpenFileDialog::Get(HINSTANCE instance, HWND parent, const char* filter)
{
    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));

    char filename[MAX_PATH];
    memset(filename, 0, sizeof(char)*MAX_PATH);

    ofn.lStructSize     = sizeof(OPENFILENAME);
    ofn.hwndOwner       = parent;
    ofn.hInstance       = instance;
    ofn.lpstrFilter     = filter;
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = MAX_PATH;
    ofn.Flags           = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

    if(GetOpenFileName(&ofn))
    {
        return std::string(filename);
    }
    return std::string();
}

std::vector<std::string> OpenFileDialog::GetMany(HINSTANCE instance, HWND parent, const char* filter)
{
    std::vector<std::string> rv;

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));

    char filename[2048];
    memset(filename, 0, sizeof(char)*2048);

    ofn.lStructSize     = sizeof(OPENFILENAME);
    ofn.hwndOwner       = parent;
    ofn.hInstance       = instance;
    ofn.lpstrFilter     = filter;
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = 2048;
    ofn.Flags           = OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

    if(GetOpenFileName(&ofn))
    {
        // From MSDN:
        //      If the OFN_ALLOWMULTISELECT flag is set and the user selects multiple files, the
        //      buffer contains the current directory followed by the file names of the selected
        //      files. For Explorer-style dialog boxes, the directory and file name strings are
        //      NULL separated, with an extra NULL character after the last file name. For old-style
        //      dialog boxes, the strings are space separated and the function uses short file names
        //      for file names with spaces. You can use the FindFirstFile function to convert between
        //      long and short file names. If the user selects only one file, the lpstrFile string
        //      does not have a separator between the path and file name.

        // Where is the first NULL character?
        size_t null_pos = strlen(filename);

        // Is the index of the first character of the first filename passed this NULL character?
        if(ofn.nFileOffset > null_pos)
        {
            // Yes, then the user has selected multiple files
            std::string full_path = std::string(filename) + "\\";
            char* ptr = filename + null_pos + 1;
            while(*ptr)
            {
                std::string temp = full_path + std::string(ptr);
                rv.push_back(temp);
                ptr += strlen(ptr) + 1;
            }
        }
        else
        {
            // No, then the user has selected only one file.
            rv.push_back(filename);
        }
    }

    return rv;
}
