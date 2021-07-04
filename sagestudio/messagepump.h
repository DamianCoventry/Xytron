#ifndef INCLUDED_MESSAGEPUMP
#define INCLUDED_MESSAGEPUMP

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

class MessagePump
{
public:
    MessagePump(HINSTANCE instance)
        : instance_(instance) {}
    void Wait(HWND window, int accelerator_id);
private:
    HINSTANCE instance_;
};

#endif  // INCLUDED_MESSAGEPUMP
