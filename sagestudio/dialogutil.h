#ifndef INCLUDED_DIALOGUTIL
#define INCLUDED_DIALOGUTIL

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

namespace DialogUtil
{

void CenterInParent(HWND window);

}       // namespace DialogUtil

#endif  // INCLUDED_DIALOGUTIL
