#ifndef INCLUDED_TEMPLATEWINDOW
#define INCLUDED_TEMPLATEWINDOW

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>

struct TemplateWindowEvents
{
    virtual void OnTemplateWindowControlClicked() = 0;
};

class TemplateWindow
{
public:
    TemplateWindow(HINSTANCE instance, HWND parent, TemplateWindowEvents* event_handler);
    ~TemplateWindow();

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

private:
    void Register();
    void Unregister();
    void Create(HWND parent);
    void Delete();

    friend LRESULT CALLBACK TemplateWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    static const char* window_title_;
    static const char* class_name_;

    HINSTANCE instance_;
    HWND handle_;
    TemplateWindowEvents* event_handler_;
};

typedef boost::shared_ptr<TemplateWindow> TemplateWindowPtr;

#endif  // INCLUDED_TEMPLATEWINDOW
