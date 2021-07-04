#ifndef INCLUDED_DEFAULTBINDINGSDIALOGBAR
#define INCLUDED_DEFAULTBINDINGSDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>

#include "../sagedocuments/cameradocument.h"
#include "../sagedocuments/bindingsfile.h"

struct DefaultBindingsDialogBarEvents
{
    virtual void OnDefaultBindingsChanged(const std::string& binding, const std::string& value) = 0;
    virtual void OnDefaultBindingsRemoved(const std::string& binding) = 0;
    virtual void OnAllDefaultBindingsCleared() = 0;
};

class DefaultBindingsDialogBar
{
public:
    DefaultBindingsDialogBar(HINSTANCE instance, HWND parent, DefaultBindingsDialogBarEvents* event_handler);
    ~DefaultBindingsDialogBar();

    void CameraDocuments(CameraDocumentMap* docs)   { cam_docs_ = docs; }
    void BindingsDocument(BindingsFile* docs);
    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

private:
    friend INT_PTR CALLBACK DefaultBindingsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnButtonSet();
    void OnButtonClear();
    void OnButtonClearAll();
    void OnDoubleClickBindingsListView(int item_index, int sub_item_index);

private:
    CameraDocumentMap* cam_docs_;
    BindingsFile* bind_docs_;
    DefaultBindingsDialogBarEvents* event_handler_;
    HINSTANCE instance_;
    HWND handle_;
};

typedef boost::shared_ptr<DefaultBindingsDialogBar> DefaultBindingsDialogBarPtr;

#endif  // INCLUDED_DEFAULTBINDINGSDIALOGBAR
