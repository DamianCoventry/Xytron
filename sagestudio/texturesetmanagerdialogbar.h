#ifndef INCLUDED_TEXTURESETMANAGERDIALOGBAR
#define INCLUDED_TEXTURESETMANAGERDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>

#include <vector>
#include "worldtypes.h"
#include "../gfx/color.h"

struct TextureSetManagerDialogBarEvents
{
    virtual void OnTextureSetSet(TextureSetChannel c, const std::string& filename) = 0;
    virtual void OnTextureSetRemove(TextureSetChannel c) = 0;
    virtual void OnTextureSetClearAll() = 0;
    virtual void OnTextureSetSetLiquidColor(const Gfx::Color& color) = 0;
};

class TextureSetDocument;

namespace MyTest
{
    class DibResources;
}

class TextureSetManagerDialogBar
{
public:
    TextureSetManagerDialogBar(HINSTANCE instance, HWND parent, TextureSetManagerDialogBarEvents* event_handler);
    ~TextureSetManagerDialogBar();

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

    void SetTextureSetDocument(TextureSetDocument* doc);
    void DibResources(MyTest::DibResources* dib_resources)      { dib_resources_ = dib_resources; }
    void ContentDirectory(const std::string& content_dir)       { content_dir_ = content_dir; }

private:
    friend INT_PTR CALLBACK TextureSetManagerDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item);
    void OnListboxDrawItem(DRAWITEMSTRUCT* draw_item);
    void OnEditBoxColorChanged();

    void OnButtonSet();
    void OnButtonRemove();
    void OnButtonClearAll();

private:
    TextureSetManagerDialogBarEvents* event_handler_;
    TextureSetDocument* ts_doc_;
    HINSTANCE instance_;
    HWND handle_;
    HFONT od_font_;
    MyTest::DibResources* dib_resources_;
    std::string content_dir_;
    std::vector<std::string> filenames_;
};

typedef boost::shared_ptr<TextureSetManagerDialogBar> TextureSetManagerDialogBarPtr;

#endif  // INCLUDED_TEXTURESETMANAGERDIALOGBAR
