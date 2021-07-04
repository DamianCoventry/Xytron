#ifndef INCLUDED_CHOOSEMUSICDIALOG
#define INCLUDED_CHOOSEMUSICDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

namespace MyTest
{
class DibResources;
}

class ChooseMusicDialog
{
public:
    ChooseMusicDialog();
    ~ChooseMusicDialog();

    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }

    void MusicFilename(const std::string& music_filename)   { music_filename_ = music_filename; }
    const std::string& MusicFilename() const                { return music_filename_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseMusicDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string music_filename_;
    std::string content_dir_;
};

#endif  // INCLUDED_CHOOSEMUSICDIALOG
