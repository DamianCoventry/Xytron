#ifndef INCLUDED_STATENODEPROPERTIESDIALOG
#define INCLUDED_STATENODEPROPERTIESDIALOG

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include "../sagedocuments/imagedocument.h"
#include "../sagedocuments/imageanimationdocument.h"
#include "../sagedocuments/resourcecontextdocument.h"
#include "../sagedocuments/cellmapdocument.h"
#include "../sagedocuments/cameradocument.h"

#include <string>

namespace MyTest
{
class DibResources;
}

class SmStateDocument;

class StateNodePropertiesDialog
{
public:
    StateNodePropertiesDialog();

    void Name(const std::string& name)          { name_ = name; }

    void SetImageAnimationDocumentMap(ImageAnimationDocumentMap* image_anim_map)    { image_anim_map_ = image_anim_map; }
    void SetImageDocumentMap(ImageDocumentMap* image_map)                           { image_map_ = image_map; }
    void DibResources(MyTest::DibResources* dib_resources)                          { dib_resources_ = dib_resources; }
    void SetStateDocument(SmStateDocument* state_doc)                               { state_doc_ = state_doc; }
    void ContentDirectory(const std::string& content_dir)                                   { content_dir_ = content_dir; }
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map)    { res_ctxt_map_ = res_ctxt_map; }
    void SetCellMapDocumentList(CellMapDocumentList* cell_map_list) { cell_map_list_ = cell_map_list; }
    void SetCameraDocuments(CameraDocumentMap* cam_map)                             { cam_map_ = cam_map; }

    std::string Name() const { return name_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK StateNodePropertiesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();
    void OnButtonInsert();
    void OnButtonEdit();
    void OnButtonRemove();
    void OnButtonClearAll();
    void OnButtonMoveUp();
    void OnButtonMoveDown();
    void OnComboEventsChanged();

private:
    HWND dialog_;
    std::string name_;
    ImageAnimationDocumentMap* image_anim_map_;
    ImageDocumentMap* image_map_;
    MyTest::DibResources* dib_resources_;
    SmStateDocument* state_doc_;
    std::string content_dir_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    CellMapDocumentList* cell_map_list_;
    CameraDocumentMap* cam_map_;
};

#endif  // INCLUDED_STATENODEPROPERTIESDIALOG
