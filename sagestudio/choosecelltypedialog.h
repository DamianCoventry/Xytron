#ifndef INCLUDED_CHOOSECELLTYPEDIALOG
#define INCLUDED_CHOOSECELLTYPEDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "worldtypes.h"

class ChooseCellTypeDialog
{
public:
    ChooseCellTypeDialog()
        : cell_type_(CT_SOLID_SPACE) {}

    void SetCellType(CellType cell_type)    { cell_type_ = cell_type; }
    CellType GetCellType() const            { return cell_type_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseCellTypeDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    CellType cell_type_;
};

#endif  // INCLUDED_CHOOSECELLTYPEDIALOG
