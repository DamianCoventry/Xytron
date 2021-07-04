#ifndef INCLUDED_SAVEMAPPROGRESSDIALOG
#define INCLUDED_SAVEMAPPROGRESSDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

class CellMapDocument;

class SaveMapProgressDialog
{
public:
    void SetCellMapDocument(CellMapDocument* document)              { document_ = document; }
    void SetFilePath(const std::string& path)                       { path_ = path; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK SaveMapProgressDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnWindowTimer();

    void OnCellMapFileTotal(int total);
    void OnCellMapFileStep(int value);

private:
    HWND dialog_;
    CellMapDocument* document_;
    std::string path_;
};

#endif  // INCLUDED_SaveMAPPROGRESSDIALOG
