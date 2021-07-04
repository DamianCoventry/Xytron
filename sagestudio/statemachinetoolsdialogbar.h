#ifndef INCLUDED_STATEMACHINETOOLSDIALOGBAR
#define INCLUDED_STATEMACHINETOOLSDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <boost/shared_ptr.hpp>

struct StateMachineToolsDialogBarEvents
{
    virtual void OnStateMachineSelect() = 0;
    virtual void OnStateMachineInsertEntry() = 0;
    virtual void OnStateMachineInsertExit() = 0;
    virtual void OnStateMachineInsertState() = 0;
    virtual void OnStateMachineInsertEvent() = 0;
    virtual void OnStateMachineDelete() = 0;
    virtual void OnStateMachineProperties() = 0;
    virtual void OnStateMachineClearAll() = 0;
};

class StateMachineToolsDialogBar
{
public:
    StateMachineToolsDialogBar(HINSTANCE instance, HWND parent, StateMachineToolsDialogBarEvents* event_handler);
    ~StateMachineToolsDialogBar();

    void RepositionWithinParent();

    void Enable();
    void Disable();
    void Show();
    void Hide();

private:
    friend INT_PTR CALLBACK StateMachineToolsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonInsert();
    void OnButtonClearAll();
    void OnButtonProperties();

private:
    HINSTANCE instance_;
    HWND handle_;
    StateMachineToolsDialogBarEvents* event_handler_;
};

typedef boost::shared_ptr<StateMachineToolsDialogBar> StateMachineToolsDialogBarPtr;

#endif  // INCLUDED_STATEMACHINETOOLSDIALOGBAR
