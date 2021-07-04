#ifndef INCLUDED_CELLMAPVIEWDIALOGBAR
#define INCLUDED_CELLMAPVIEWDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>

#include "worldtypes.h"

struct CellMapViewDialogBarEvents
{
    virtual void OnCellMapViewDialogShowGridChanged(bool show) = 0;
    virtual void OnCellMapViewDialogBarViewStyleChanged(ViewStyle vs) = 0;
    virtual void OnCellMapViewDialogBarEditModeChanged(EditMode em) = 0;
    virtual void OnCellMapViewDialogBarZoomLevelChanged(int zoom) = 0;
};

class CellMapViewDialogBar
{
public:
    CellMapViewDialogBar(HINSTANCE instance, HWND parent, CellMapViewDialogBarEvents* event_handler);
    ~CellMapViewDialogBar();

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

    void SetViewStyle(ViewStyle vs);
    void SetEditMode(EditMode em);
    void SetZoomLevel(int zoom);

private:
    friend INT_PTR CALLBACK CellMapViewDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnComboBoxViewStyleChanged();
    void OnComboBoxEditModeChanged();
    void OnComboBoxZoomLevelChanged();

private:
    CellMapViewDialogBarEvents* event_handler_;
    HINSTANCE instance_;
    HWND handle_;
};

typedef boost::shared_ptr<CellMapViewDialogBar> CellMapViewDialogBarPtr;

#endif  // INCLUDED_CELLMAPVIEWDIALOGBAR
