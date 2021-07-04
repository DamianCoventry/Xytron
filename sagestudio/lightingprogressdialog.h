#ifndef INCLUDED_LIGHTINGPROGRESSDIALOG
#define INCLUDED_LIGHTINGPROGRESSDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class CellMapDocument;

class LightingProgressDialog
{
public:
    void SetCellMapDocument(CellMapDocument* document) { document_ = document; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK LightingProgressDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnWindowTimer();

    void OnTotal(int total);
    void OnStep(int value);

private:
    HWND dialog_;
    CellMapDocument* document_;
};

#endif  // INCLUDED_LightingPROGRESSDIALOG
