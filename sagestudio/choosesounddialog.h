#ifndef INCLUDED_CHOOSESOUNDDIALOG
#define INCLUDED_CHOOSESOUNDDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

namespace MyTest
{
class DibResources;
}

class ChooseSoundDialog
{
public:
    ChooseSoundDialog();
    ~ChooseSoundDialog();

    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }

    void SoundFilename(const std::string& sound_filename)   { sound_filename_ = sound_filename; }
    const std::string& SoundFilename() const                { return sound_filename_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseSoundDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string sound_filename_;
    std::string content_dir_;
};

#endif  // INCLUDED_CHOOSESOUNDDIALOG
