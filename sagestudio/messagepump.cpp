#include "messagepump.h"

void MessagePump::Wait(HWND window, int accelerator_id)
{
    HACCEL accel_table = LoadAccelerators(instance_, MAKEINTRESOURCE(accelerator_id));

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        if(!TranslateAccelerator(window, accel_table, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DestroyAcceleratorTable(accel_table);
}
