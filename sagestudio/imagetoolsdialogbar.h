#ifndef INCLUDED_IMAGETOOLSDIALOGBAR
#define INCLUDED_IMAGETOOLSDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <boost/shared_ptr.hpp>

struct ImageToolsDialogBarEvents
{
    enum Tool { T_MOVEIMAGE, T_RESIZEIMAGE, T_MOVEVIEW };
    virtual void OnImageToolsDialogBarZoomChanged(int zoom) = 0;
    virtual void OnImageToolsDialogBarToolChanged(Tool tool) = 0;
    virtual void OnImageToolsDialogBarEnableGridSnapChanged(bool enabled) = 0;
    virtual void OnImageToolsDialogBarGridSnapChanged(int grid_snap) = 0;
};

class ImageToolsDialogBar
{
public:
    ImageToolsDialogBar(HINSTANCE instance, HWND parent, ImageToolsDialogBarEvents* event_handler);
    ~ImageToolsDialogBar();

    void Zoom(int zoom);
    void SetTool(ImageToolsDialogBarEvents::Tool tool);
    void EnableGridSnap(bool enable);
    void GridSnap(int grid_snap);

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

private:
    friend INT_PTR CALLBACK ImageToolsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnComboBoxZoomChanged();
    void OnComboBoxToolChanged();
    void OnCheckBoxEnableGridSnap();
    void OnButtonApplyGridSnap();

private:
    ImageToolsDialogBarEvents* event_handler_;
    HINSTANCE instance_;
    HWND handle_;
    int zoom_;
    ImageToolsDialogBarEvents::Tool tool_;
    bool grid_snap_enabled_;
    int grid_snap_;
};

typedef boost::shared_ptr<ImageToolsDialogBar> ImageToolsDialogBarPtr;

#endif  // INCLUDED_IMAGETOOLSDIALOGBAR
