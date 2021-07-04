#ifndef INCLUDED_COMMANDDIALOG
#define INCLUDED_COMMANDDIALOG

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include "../sagedocuments/imagedocument.h"
#include "../sagedocuments/imageanimationdocument.h"
#include "../sagedocuments/resourcecontextdocument.h"
#include "../sagedocuments/cellmapdocument.h"
#include "../sagedocuments/cameradocument.h"

#include <string>
#include <vector>

namespace MyTest
{
class DibResources;
}

class CommandDialog
{
public:
    CommandDialog();

    void SetImageAnimationDocumentMap(ImageAnimationDocumentMap* image_anim_map)    { image_anim_map_ = image_anim_map; }
    void SetImageDocumentMap(ImageDocumentMap* image_map)   { image_map_ = image_map; }
    void DibResources(MyTest::DibResources* dib_resources)  { dib_resources_ = dib_resources; }
    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void SetCellMapDocumentList(CellMapDocumentList* cell_map_list) { cell_map_list_ = cell_map_list; }
    void SetCameraDocuments(CameraDocumentMap* cam_map)     { cam_map_ = cam_map; }

    void Command(const std::string& command)                    { command_ = command; }
    void Parameters(const std::vector<std::string>& parameters) { parameters_ = parameters; }

    const std::string& Command() const                  { return command_; }
    const std::vector<std::string>& Parameters() const  { return parameters_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK CommandDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();
    void OnComboCommandTypeChanged();
    void OnClickLinkControl(const std::wstring& link_id);

private:
    static const int IDC_LINK_ID;
    ImageAnimationDocumentMap* image_anim_map_;
    ImageDocumentMap* image_map_;
    HWND dialog_;
    HWND link_control_;
    std::string command_;
    std::string content_dir_;
    std::vector<std::string> parameters_;
    MyTest::DibResources* dib_resources_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    CellMapDocumentList* cell_map_list_;
    CameraDocumentMap* cam_map_;
};

#endif  // INCLUDED_COMMANDDIALOG
