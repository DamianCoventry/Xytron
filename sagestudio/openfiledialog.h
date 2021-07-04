#ifndef INCLUDED_OPENFILEDIALOG
#define INCLUDED_OPENFILEDIALOG

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include <string>
#include <vector>

struct OpenFileDialog
{
    static std::string Get(HINSTANCE instance, HWND parent, const char* filter);
    static std::vector<std::string> GetMany(HINSTANCE instance, HWND parent, const char* filter);
};

#endif  // INCLUDED_OPENFILEDIALOG
