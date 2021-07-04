#ifndef INCLUDED_SKYDIALOGBAR
#define INCLUDED_SKYDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>

#include "worldtypes.h"

struct SkyDialogBarEvents
{
    virtual void OnSkyDialogBarCurrentBrushChanged(SkyBrush sky_brush) = 0;
    virtual void OnSkyDialogBarSetToSkyClicked() = 0;
    virtual void OnSkyDialogBarClearSkyClicked() = 0;
    virtual void OnSkyDialogBarChooseSkyTexClicked() = 0;
};

class SkyDialogBar
{
public:
    SkyDialogBar(HINSTANCE instance, HWND parent, SkyDialogBarEvents* event_handler);
    ~SkyDialogBar();

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

private:
    friend INT_PTR CALLBACK SkyDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnComboBoxCurrentBrushChanged();

private:
    SkyDialogBarEvents* event_handler_;
    HINSTANCE instance_;
    HWND handle_;
};

typedef boost::shared_ptr<SkyDialogBar> SkyDialogBarPtr;

#endif  // INCLUDED_SKYDIALOGBAR
