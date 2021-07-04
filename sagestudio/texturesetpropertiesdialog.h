#ifndef INCLUDED_TEXTURESETPROPERTIESDIALOG
#define INCLUDED_TEXTURESETPROPERTIESDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include "../sagedocuments/resourcecontextdocument.h"

class TextureSetPropertiesDialog
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* rc_map) { rc_map_ = rc_map; }

    void Name(const std::string& name)      { name_ = name; }
    void ResCxt(const std::string& res_cxt) { res_cxt_ = res_cxt; }

    const std::string& Name() const     { return name_; }
    const std::string& ResCxt() const   { return res_cxt_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK TextureSetPropertiesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    MyTest::ResourceContextDocumentMap* rc_map_;
    HWND dialog_;
    std::string name_;
    std::string res_cxt_;
};

#endif  // INCLUDED_TEXTURESETPROPERTIESDIALOG
