#include "messagepump.h"
#include "sagestudiowindow.h"
#include "resource.h"
#include "../util/log.h"

#include <objbase.h>
#include <commctrl.h>
#include <exception>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, INT cmd_show)
{
    Log::Open("SageStudio.log");

    CoInitialize(NULL);
    InitCommonControls();

    try
    {
        SageStudioWindow window(instance);
        window.Show(cmd_show);

        MessagePump pump(instance);
        pump.Wait(window.Handle(), IDR_MAIN_WIN_ACCEL);

        window.Hide();
    }
    catch(std::exception& e)
    {
        MessageBox(GetDesktopWindow(), e.what(), "Unhandled exception", MB_OK | MB_ICONINFORMATION);
    }

    CoUninitialize();
    Log::Close();
}
