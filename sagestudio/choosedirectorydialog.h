#ifndef INCLUDED_CHOOSEDIRECTORYDIALOG
#define INCLUDED_CHOOSEDIRECTORYDIALOG

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include <string>

struct ChooseDirectoryDialog
{
    static std::string Run(HWND parent, const std::string& caption);
};

#endif  // INCLUDED_CHOOSEDIRECTORYDIALOG
