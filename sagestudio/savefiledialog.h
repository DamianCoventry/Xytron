#ifndef INCLUDED_SAVEFILEDIALOG
#define INCLUDED_SAVEFILEDIALOG

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include <string>

struct SaveFileDialog
{
    static std::string Get(HINSTANCE instance, HWND parent, const char* filter, const char* default_ext, const std::string& initial_filename);
};

#endif  // INCLUDED_SAVEFILEDIALOG
