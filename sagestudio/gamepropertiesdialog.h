#ifndef INCLUDED_GAMEPROPERTIESDIALOG
#define INCLUDED_GAMEPROPERTIESDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include "../sagedocuments/statemachinedocument.h"

class GamePropertiesDialog
{
public:
    GamePropertiesDialog();

    void SetStateMachineDocumentMap(StateMachineDocumentMap* sm_docs) { sm_docs_ = sm_docs; }

    void Name(const std::string& name)                  { name_ = name; }
    void StateMachine(const std::string& state_machine) { state_machine_ = state_machine; }
    void ClearBackBuffer(bool clear_back_buffer)        { clear_back_buffer_ = clear_back_buffer; }

    const std::string& Name() const         { return name_; }
    const std::string& StateMachine() const { return state_machine_; }
    bool ClearBackBuffer() const            { return clear_back_buffer_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK GamePropertiesDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();

private:
    HWND dialog_;
    std::string name_;
    std::string state_machine_;
    bool clear_back_buffer_;
    StateMachineDocumentMap* sm_docs_;
};

#endif  // INCLUDED_GAMEPROPERTIESDIALOG
