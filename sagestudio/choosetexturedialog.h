#ifndef INCLUDED_CHOOSETEXTUREDIALOG
#define INCLUDED_CHOOSETEXTUREDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <vector>

namespace MyTest
{
class DibResources;
}

class ChooseTextureDialog
{
public:
    ChooseTextureDialog();
    ~ChooseTextureDialog();

    void DibResources(MyTest::DibResources* dib_resources)      { dib_resources_ = dib_resources; }
    void ContentDirectory(const std::string& content_dir)       { content_dir_ = content_dir; }

    void Filename(const std::string& filename)  { filename_ = filename; }
    const std::string& Filename() const         { return filename_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    std::vector<std::string> DirectoryListing(const std::string& filespec, bool use_content_dir = true);

    friend INT_PTR CALLBACK ChooseTextureDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();
    void OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item);
    void OnListboxDrawItem(DRAWITEMSTRUCT* draw_item);

private:
    HWND dialog_;
    HFONT od_font_;
    std::string filename_;
    std::string content_dir_;
    MyTest::DibResources* dib_resources_;
};

#endif  // INCLUDED_CHOOSETEXTUREDIALOG
