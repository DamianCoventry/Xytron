#ifndef INCLUDED_TEXTURESETDIALOGBAR
#define INCLUDED_TEXTURESETDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>

#include "../sagedocuments/texturesetdocument.h"
#include "worldtypes.h"

struct TextureSetDialogBarEvents
{
    virtual void OnTextureSetBrushChanged(TextureSetBrush ts_brush) = 0;
    virtual void OnTextureSetSelectNone() = 0;
    virtual void OnTextureSetSetCellType() = 0;
    virtual void OnTextureSetSetTextureSet() = 0;
    virtual void OnTextureSetTextureSetChanged(const std::string& texure_set) = 0;
    virtual void OnTextureSetDefaultTextureSetChanged(const std::string& default_texure_set) = 0;
};

class TextureSetDialogBar
{
public:
    TextureSetDialogBar(HINSTANCE instance, HWND parent, TextureSetDialogBarEvents* event_handler);
    ~TextureSetDialogBar();

    void RepositionWithinParent();

    void SetTextureSetDocumentMap(TextureSetDocumentMap* ts_docs);

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

private:
    friend INT_PTR CALLBACK TextureSetDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnComboBoxTextureSetBrushChanged();
    void OnListBoxTextureSetChanged();
    void OnComboBoxDefaultTextureSetChanged();

private:
    TextureSetDialogBarEvents* event_handler_;
    HINSTANCE instance_;
    HWND handle_;
    TextureSetBrush ts_brush_;
    TextureSetDocumentMap* ts_docs_;
    std::string default_texture_set_;
};

typedef boost::shared_ptr<TextureSetDialogBar> TextureSetDialogBarPtr;

#endif  // included_texturesetdialogbar
