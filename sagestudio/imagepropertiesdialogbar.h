#ifndef INCLUDED_IMAGEPROPERTIESDIALOGBAR
#define INCLUDED_IMAGEPROPERTIESDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace MyTest
{
class DibResources;
}

struct ImagePropertiesDialogBarEvents
{
    virtual void OnImagePropertiesDialogBarModified() = 0;
    virtual void OnImagePropertiesDialogBarTextureChanged(const std::string& filename) = 0;
    virtual void OnImagePropertiesDialogBarPositionChanged(const POINT& position) = 0;
    virtual void OnImagePropertiesDialogBarSizeChanged(const SIZE& size) = 0;
    virtual void OnImageDialogFrameCountChanged(int frame_count) = 0;
};

class ImagePropertiesDialogBar
{
public:
    ImagePropertiesDialogBar(HINSTANCE instance, HWND parent, ImagePropertiesDialogBarEvents* event_handler);
    ~ImagePropertiesDialogBar();

    void Texture(const std::string& filename);
    void Position(const POINT& position);
    void Size(const SIZE& size);
    void FrameCount(int frame_count);

    void EnableFrameCount(bool enable);

    void RepositionWithinParent();

    void DibResources(MyTest::DibResources* dib_resources)      { dib_resources_ = dib_resources; }
    void ContentDirectory(const std::string& content_dir)       { content_dir_ = content_dir; }
    void RefreshListBox();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

private:
    std::vector<std::string> DirectoryListing(const std::string& filespec, bool use_content_dir = true);

    friend INT_PTR CALLBACK ImagePropertiesDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item);
    void OnListboxDrawItem(DRAWITEMSTRUCT* draw_item);
    void OnListBoxTextureChanged();
    void OnButtonApplyPosition();
    void OnButtonApplySize();
    void OnButtonApplyFrameCount();
    void OnComboBoxActionChanged();

private:
    HINSTANCE instance_;
    HWND handle_;
    HWND list_box_;
    HFONT od_font_;
    ImagePropertiesDialogBarEvents* event_handler_;
    MyTest::DibResources* dib_resources_;
    std::string content_dir_;
};

typedef boost::shared_ptr<ImagePropertiesDialogBar> ImagePropertiesDialogBarPtr;

#endif  // INCLUDED_IMAGEPROPERTIESDIALOGBAR
