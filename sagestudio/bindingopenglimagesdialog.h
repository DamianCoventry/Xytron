#ifndef INCLUDED_BINDINGOPENGLIMAGESDIALOG
#define INCLUDED_BINDINGOPENGLIMAGESDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../sagedocuments/texturesetdocument.h"

namespace Util
{
class ResourceContext;
}

class BindingOpenGLImagesDialog
{
public:
    void SetTextureSetDocumentMap(TextureSetDocumentMap* ts_docs)   { ts_docs_ = ts_docs; }
    void SetResourceContext(Util::ResourceContext* res_cxt)         { res_cxt_ = res_cxt; }
    void ContentDirectory(const std::string& content_dir)           { content_dir_ = content_dir; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK BindingOpenGLImagesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnWindowTimer();

private:
    HWND dialog_;
    TextureSetDocumentMap* ts_docs_;
    Util::ResourceContext* res_cxt_;
    std::string content_dir_;
};

#endif  // INCLUDED_BINDINGOPENGLIMAGESDIALOG
