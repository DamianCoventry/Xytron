#ifndef INCLUDED_EMPTYSTUDIOWINDOW
#define INCLUDED_EMPTYSTUDIOWINDOW

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <boost/shared_ptr.hpp>

class EmptyStudioWindow
{
public:
    EmptyStudioWindow(HINSTANCE instance, HWND parent);
    ~EmptyStudioWindow();

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();

private:
    void Register();
    void Unregister();
    void Create(HWND parent);
    void Delete();

    friend LRESULT CALLBACK EmptyStudioWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    static const char* window_title_;
    static const char* class_name_;

    HINSTANCE instance_;
    HWND handle_;
};

typedef boost::shared_ptr<EmptyStudioWindow> EmptyStudioWindowPtr;

#endif  // INCLUDED_EMPTYSTUDIOWINDOW
