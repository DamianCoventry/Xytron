#ifndef INCLUDED_CHOOSEBINDCOMMANDDIALOG
#define INCLUDED_CHOOSEBINDCOMMANDDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>
#include <string>

#include "../sagedocuments/cameradocument.h"

class ChooseBindCommandDialog
{
public:
    ChooseBindCommandDialog();

    void CameraDocuments(CameraDocumentMap* docs)   { cam_docs_ = docs; }
    void CommandLine(const std::vector<std::string>& cl) { cl_ = cl; }

    const std::vector<std::string>& CommandLine() const { return cl_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseBindCommandDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HINSTANCE instance_;
    HWND dialog_;
    std::vector<std::string> cl_;
    CameraDocumentMap* cam_docs_;
};

#endif  // INCLUDED_CHOOSEBINDCOMMANDDIALOG
