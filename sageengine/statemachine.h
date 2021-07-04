#ifndef INCLUDED_STATEMACHINE
#define INCLUDED_STATEMACHINE

#include <string>
#include <map>
#include "state.h"

class StateMachine
{
public:
    void AddState(const std::string& name, StatePtr state);

    void SetCurrentState(const std::string& state_name);
    void TransitionToState(const std::string& state_name);
    void DoPendingStateChange();

    void CallOnThinkCommandLineSet(float time_delta);
    void CallOnMoveCommandLineSet(float time_delta);
    void CallOnDrawCommandLineSet();
    void CallOnTimerCommandLineSet();

private:
    StatePtr current_state_;
    StatePtr pending_state_;

    typedef std::map<std::string, StatePtr> StateMap;
    StateMap available_states_;
};

#endif  // INCLUDED_STATEMACHINE
