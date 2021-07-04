#ifndef INCLUDED_CELLMAPPROPSDIALOGBAR
#define INCLUDED_CELLMAPPROPSDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>
#include "worldtypes.h"

struct CellMapPropsDialogBarEvents
{
    virtual void OnCellMapCellBrushChanged(CellBrush cell_brush) = 0;
    virtual void OnCellMapSelectNone() = 0;
    virtual void OnCellMapSetCellType() = 0;
    virtual void OnCellMapSetTextureSet() = 0;
};

class CellMapPropsDialogBar
{
public:
    CellMapPropsDialogBar(HINSTANCE instance, HWND parent, CellMapPropsDialogBarEvents* event_handler);
    ~CellMapPropsDialogBar();

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

    void SetCellBrush(CellBrush brush);
    CellBrush GetCellBrush() const      { return cell_brush_; }

private:
    friend INT_PTR CALLBACK CellMapPropsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnComboBoxCellBrushChanged();

private:
    CellMapPropsDialogBarEvents* event_handler_;
    HINSTANCE instance_;
    HWND handle_;
    CellBrush cell_brush_;
};

typedef boost::shared_ptr<CellMapPropsDialogBar> CellMapPropsDialogBarPtr;

#endif  // INCLUDED_CELLMAPPROPSDIALOGBAR
