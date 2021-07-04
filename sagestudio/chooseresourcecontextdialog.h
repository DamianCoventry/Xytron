#ifndef INCLUDED_CHOOSERESOURCECONTEXTDIALOG
#define INCLUDED_CHOOSERESOURCECONTEXTDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include "../sagedocuments/resourcecontextdocument.h"

namespace MyTest
{
class DibResources;
}

class ChooseResourceContextDialog
{
public:
    ChooseResourceContextDialog();
    ~ChooseResourceContextDialog();

    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }

    void ResourceContextName(const std::string& resource_context_name)  { resource_context_name_ = resource_context_name; }
    const std::string& ResourceContextName() const                      { return resource_context_name_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseResourceContextDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string resource_context_name_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
};

#endif  // INCLUDED_CHOOSERESOURCECONTEXTDIALOG
