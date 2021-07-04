#ifndef INCLUDED_CHOOSETEXTURESETDIALOG
#define INCLUDED_CHOOSETEXTURESETDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include "../sagedocuments/texturesetdocument.h"

class ChooseTextureSetDialog
{
public:
    ChooseTextureSetDialog();
    ~ChooseTextureSetDialog();

    void SetTextureSetDocumentMap(TextureSetDocumentMap* ts_docs)   { ts_docs_ = ts_docs; }

    const std::string& TextureSet() const { return ts_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseTextureSetDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string ts_;
    TextureSetDocumentMap* ts_docs_;
};

#endif  // INCLUDED_CHOOSETextureSetDIALOG
