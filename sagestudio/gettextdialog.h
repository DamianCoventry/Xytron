#ifndef INCLUDED_GETTEXTDIALOG
#define INCLUDED_GETTEXTDIALOG

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include <string>

class GetTextDialog
{
public:
    void WindowTitle(const std::string& title)  { title_ = title; }
    void Label(const std::string& label)        { label_ = label; }
    void Text(const std::string& text)          { text_ = text; }

    std::string Text() const { return text_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK GetTextDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string title_;
    std::string label_;
    std::string text_;
};

#endif  // INCLUDED_GETTEXTDIALOG
