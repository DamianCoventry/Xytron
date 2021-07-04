#ifndef INCLUDED_LOADMAPPROGRESSDIALOG
#define INCLUDED_LOADMAPPROGRESSDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include "../sagedocuments/texturesetdocument.h"

namespace Util
{
    class ResourceContext;
}
class CellMapDocument;

class LoadMapProgressDialog
{
public:
    void SetResourceContext(Util::ResourceContext* res_cxt)         { res_cxt_ = res_cxt; }
    void SetTextureSetDocumentMap(TextureSetDocumentMap* ts_docs)   { ts_docs_ = ts_docs; }
    void SetContentDir(const std::string& content_dir)              { content_dir_ = content_dir; }
    void SetCellMapDocument(CellMapDocument* document)              { document_ = document; }
    void SetFilePath(const std::string& path)                       { path_ = path; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK LoadMapProgressDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnWindowTimer();

    void OnBindTexturesTotal(int total);
    void OnBindTexturesStep(int value);

    void OnCellMapFileTotal(int total);
    void OnCellMapFileStep(int value);

    void OnBuildCombinedCellBspTreesTotal(int total);
    void OnBuildCombinedCellBspTreesStep(int value);

private:
    HWND dialog_;
    Util::ResourceContext* res_cxt_;
    TextureSetDocumentMap* ts_docs_;
    std::string content_dir_;
    CellMapDocument* document_;
    std::string path_;
};

#endif  // INCLUDED_LOADMAPPROGRESSDIALOG
