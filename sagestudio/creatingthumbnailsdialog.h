#ifndef INCLUDED_CREATINGTHUMBNAILSDIALOG
#define INCLUDED_CREATINGTHUMBNAILSDIALOG

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <vector>
#include <string>

#include "../sagedocuments/imagedocument.h"
#include "../sagedocuments/imageanimationdocument.h"

namespace MyTest
{
class DibResources;
}

class CreatingThumbnailsDialog
{
public:
    void DibResources(MyTest::DibResources* dib_resources)                      { dib_resources_ = dib_resources; }
    void ImageDocuments(ImageDocumentMap* img_doc_map)                          { img_doc_map_ = img_doc_map; }
    void ImageAnimationDocuments(ImageAnimationDocumentMap* img_anim_doc_map)   { img_anim_doc_map_ = img_anim_doc_map; }
    void ContentDirectory(const std::string& content_dir)                       { content_dir_ = content_dir; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    std::vector<std::string> DirectoryListing(const std::string& filespec);

private:
    friend INT_PTR CALLBACK CreatingThumbnailsDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnWindowTimer();

private:
    HWND dialog_;
    std::string content_dir_;
    MyTest::DibResources*       dib_resources_;
    ImageDocumentMap*           img_doc_map_;
    ImageAnimationDocumentMap*  img_anim_doc_map_;
};

#endif  // INCLUDED_CREATINGTHUMBNAILSDIALOG
