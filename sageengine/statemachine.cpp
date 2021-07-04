#include "statemachine.h"

void StateMachine::AddState(const std::string& name, StatePtr state)
{
    available_states_[name] = state;
}





void StateMachine::SetCurrentState(const std::string& state_name)
{
    if(current_state_)
    {
        current_state_->CallOnEndCommandLineSet();
    }

    StateMap::iterator itor = available_states_.find(state_name);
    if(itor == available_states_.end())
    {
        throw std::runtime_error(std::string("The state named [") + state_name + "] could not be found in the map of available states");
    }

    current_state_ = itor->second;
    current_state_->CallOnBeginCommandLineSet();
}

void StateMachine::TransitionToState(const std::string& state_name)
{
    StateMap::iterator itor = available_states_.find(state_name);
    if(itor == available_states_.end())
    {
        throw std::runtime_error(std::string("The state named [") + state_name + "] could not be found in the map of available states");
    }

    pending_state_ = itor->second;
}

void StateMachine::DoPendingStateChange()
{
    if(pending_state_)
    {
        if(current_state_)
        {
            current_state_->CallOnEndCommandLineSet();
        }

        current_state_ = pending_state_;
        current_state_->CallOnBeginCommandLineSet();

        pending_state_.reset();
    }
}





void StateMachine::CallOnThinkCommandLineSet(float time_delta)
{
    if(current_state_)
    {
        current_state_->CallOnThinkCommandLineSet(time_delta);
    }
}

void StateMachine::CallOnMoveCommandLineSet(float time_delta)
{
    if(current_state_)
    {
        current_state_->CallOnMoveCommandLineSet(time_delta);
    }
}

void StateMachine::CallOnDrawCommandLineSet()
{
    if(current_state_)
    {
        current_state_->CallOnDrawCommandLineSet();
    }
}

void StateMachine::CallOnTimerCommandLineSet()
{
    if(current_state_)
    {
        current_state_->CallOnTimerCommandLineSet();
    }
}
