#ifndef INCLUDED_RESOURCECONTEXTDIALOGBAR
#define INCLUDED_RESOURCECONTEXTDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include "../sagedocuments/resourcecontextdocument.h"
#include "../sagedocuments/imagedocument.h"
#include "../sagedocuments/imageanimationdocument.h"

namespace MyTest
{
class DibResources;
}

struct ResourceContextDialogBarEvents
{
    virtual void OnResourceContextModified() = 0;
};

class ResourceContextDialogBar
{
public:
    ResourceContextDialogBar(HINSTANCE instance, HWND parent, ResourceContextDialogBarEvents* event_handler);
    ~ResourceContextDialogBar();

    void RepositionWithinParent();

    void ResourceContextDocument(MyTest::ResourceContextDocument* rc_doc);

    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }
    void DibResources(MyTest::DibResources* dib_resources)  { dib_resources_ = dib_resources; }

    void ImageDocuments(ImageDocumentMap* img_doc_map)                          { img_doc_map_ = img_doc_map; }
    void ImageAnimationDocuments(ImageAnimationDocumentMap* img_anim_doc_map)   { img_anim_doc_map_ = img_anim_doc_map; }

    void Clear();

    void Enable();
    void Disable();
    void Show();
    void Hide();

private:
    void RefreshResourceContextListBox();
    void RefreshResourceListBoxes();
    std::vector<std::string> DirectoryListing(const std::string& filespec, bool use_content_dir = true);
    bool HasResourceBeenAdded(const std::string& name, MyTest::ResourceType res_type) const;

    friend INT_PTR CALLBACK ResourceContextDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonManage();
    void OnComboBoxResourceTypeChanged();
    void OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item);
    void OnListboxDrawItem(DRAWITEMSTRUCT* draw_item);
    void OnButtonMove1Left();
    void OnButtonMoveAllLeft();
    void OnButtonMove1Right();
    void OnButtonMoveAllRight();

private:
    HINSTANCE instance_;
    HWND handle_;
    HWND list_box_;
    HFONT od_font_;

    bool managing_;
    std::string content_dir_;

    MyTest::ResourceContextDocument*    rc_doc_;
    MyTest::DibResources*               dib_resources_;
    ImageDocumentMap*                   img_doc_map_;
    ImageAnimationDocumentMap*          img_anim_doc_map_;
    ResourceContextDialogBarEvents*     event_handler_;
};

typedef boost::shared_ptr<ResourceContextDialogBar> ResourceContextDialogBarPtr;

#endif  // INCLUDED_RESOURCECONTEXTDIALOGBAR
