#include "state.h"

State::State(StateMachine* state_machine)
: state_machine_(state_machine)
{
}

void State::CallOnBeginCommandLineSet()
{
    on_begin_cls_.Execute();
}

void State::CallOnEndCommandLineSet()
{
    on_end_cls_.Execute();
}

void State::CallOnThinkCommandLineSet(float time_delta)
{
    on_think_cls_.Execute(time_delta);
}

void State::CallOnMoveCommandLineSet(float time_delta)
{
    on_move_cls_.Execute(time_delta);
}

void State::CallOnDrawCommandLineSet()
{
    on_draw_cls_.Execute();
}

void State::CallOnTimerCommandLineSet()
{
    on_timer_cls_.Execute();
}
