#ifndef INCLUDED_CAMERAPROPERTIESDIALOG
#define INCLUDED_CAMERAPROPERTIESDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

#include "../math/vector.h"

class CameraPropertiesDialog
{
public:
    const std::string& Name() const         { return name_; }
    const Math::Vector& Position() const    { return position_; }
    const Math::Vector& Rotation() const    { return rotation_; }

    void Name(const std::string& name)          { name_ = name; }
    void Position(const Math::Vector& position) { position_ = position; }
    void Rotation(const Math::Vector& rotation) { rotation_ = rotation; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK CameraPropertiesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string name_;
    Math::Vector position_;
    Math::Vector rotation_;
};

#endif  // INCLUDED_CAMERAPROPERTIESDIALOG
