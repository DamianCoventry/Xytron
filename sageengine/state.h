#ifndef INCLUDED_STATE
#define INCLUDED_STATE

#include "commandlineset.h"

#include <boost/shared_ptr.hpp>

class StateMachine;

class State
{
public:
    State(StateMachine* state_machine);

    CommandLineSet& OnBeginCommandLineSet() const   { return (CommandLineSet&)on_begin_cls_; }
    CommandLineSet& OnEndCommandLineSet() const     { return (CommandLineSet&)on_end_cls_; }
    CommandLineSet& OnThinkCommandLineSet() const   { return (CommandLineSet&)on_think_cls_; }
    CommandLineSet& OnMoveCommandLineSet() const    { return (CommandLineSet&)on_move_cls_; }
    CommandLineSet& OnDrawCommandLineSet() const    { return (CommandLineSet&)on_draw_cls_; }
    CommandLineSet& OnTimerCommandLineSet() const   { return (CommandLineSet&)on_timer_cls_; }

    void CallOnBeginCommandLineSet();
    void CallOnEndCommandLineSet();
    void CallOnThinkCommandLineSet(float time_delta);
    void CallOnMoveCommandLineSet(float time_delta);
    void CallOnDrawCommandLineSet();
    void CallOnTimerCommandLineSet();

private:
    StateMachine* state_machine_;
    CommandLineSet on_begin_cls_;
    CommandLineSet on_end_cls_;
    CommandLineSet on_think_cls_;
    CommandLineSet on_move_cls_;
    CommandLineSet on_draw_cls_;
    CommandLineSet on_timer_cls_;
};

typedef boost::shared_ptr<State> StatePtr;

#endif  // INCLUDED_STATE
