#ifndef INCLUDED_TEMPLATEDIALOGBAR
#define INCLUDED_TEMPLATEDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>

struct TemplateDialogBarEvents
{
    virtual void OnTemplateDialogBarControlClicked() = 0;
};

class TemplateDialogBar
{
public:
    TemplateDialogBar(HINSTANCE instance, HWND parent, TemplateDialogBarEvents* event_handler);
    ~TemplateDialogBar();

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

private:
    friend INT_PTR CALLBACK TemplateDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();

private:
    TemplateDialogBarEvents* event_handler_;
    HINSTANCE instance_;
    HWND handle_;
};

typedef boost::shared_ptr<TemplateDialogBar> TemplateDialogBarPtr;

#endif  // INCLUDED_TEMPLATEDIALOGBAR
