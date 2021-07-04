#ifndef INCLUDED_CONTENTDIRECTORYDIALOG
#define INCLUDED_CONTENTDIRECTORYDIALOG

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "../aud/aud.h"

namespace MyTest
{
class DibResources;
}

///////////////////////////////////////////////////////////////////////////////
class ContentDirectoryDialog
{
public:
    ContentDirectoryDialog();
    ~ContentDirectoryDialog();

    void AudioDevice(Aud::Device* device)                   { audio_device_ = device; }
    void DibResources(MyTest::DibResources* dib_resources)  { dib_resources_ = dib_resources; }

    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }
    const std::string& ContentDirectory() const             { return content_dir_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    void RefreshListBox();
    std::vector<std::string> DirectoryListing(const std::string& filespec, bool use_content_dir = true);

    friend INT_PTR CALLBACK ContentDirectoryDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonChooseContentDirectory();
    void OnComboBoxContentTypeChanged();
    void OnButtonAddFile();
    void OnButtonAddDirectory();
    void OnButtonDeleteFile();
    void OnButtonDeleteAllFiles();
    void OnButtonPlayFile();
    void OnButtonStopFile();
    void OnListboxSelectionChanged();
    void OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item);
    void OnListboxDrawItem(DRAWITEMSTRUCT* draw_item);

private:
    HINSTANCE instance_;
    HWND dialog_;
    HFONT od_font_;
    std::string content_dir_;
    Aud::Device* audio_device_;
    Aud::SoundPtr sound_;
    Aud::Music music_;
    MyTest::DibResources* dib_resources_;
};

#endif  // INCLUDED_CONTENTDIRECTORYDIALOG
