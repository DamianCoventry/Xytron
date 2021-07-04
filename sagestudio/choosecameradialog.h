#ifndef INCLUDED_CHOOSECAMERADIALOG
#define INCLUDED_CHOOSECAMERADIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

#include "../sagedocuments/cameradocument.h"

class ChooseCameraDialog
{
public:
    ChooseCameraDialog();
    ~ChooseCameraDialog();

    void CameraDocuments(CameraDocumentMap* docs)   { docs_ = docs; }

    void Camera(const std::string& camera)  { camera_ = camera; }
    const std::string& Camera() const       { return camera_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseCameraDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string camera_;
    CameraDocumentMap* docs_;
};

#endif  // INCLUDED_CHOOSECAMERADIALOG
