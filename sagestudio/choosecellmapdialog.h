#ifndef INCLUDED_CHOOSECELLMAPDIALOG
#define INCLUDED_CHOOSECELLMAPDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include "../sagedocuments/cellmapdocument.h"

class ChooseCellMapDialog
{
public:
    ChooseCellMapDialog();
    ~ChooseCellMapDialog();

    void SetCellMapDocumentList(CellMapDocumentList* cell_map_list) { cell_map_list_ = cell_map_list; }

    void CellMapName(const std::string& cell_map_name)  { cell_map_name_ = cell_map_name; }
    const std::string& CellMapName() const              { return cell_map_name_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseCellMapDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string cell_map_name_;
    CellMapDocumentList* cell_map_list_;
};

#endif  // INCLUDED_CHOOSECELLMAPDIALOG
