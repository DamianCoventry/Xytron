#include "statemachinedocument.h"

SmStateDocument::SmStateDocument()
{
    command_line_sets_["OnBegin"]   = DocCommandLineSet();
    command_line_sets_["OnEnd"]     = DocCommandLineSet();
    command_line_sets_["OnThink"]   = DocCommandLineSet();
    command_line_sets_["OnMove"]    = DocCommandLineSet();
    command_line_sets_["OnDraw"]    = DocCommandLineSet();
    command_line_sets_["OnTimer"]   = DocCommandLineSet();
}

void StateMachineDocument::Clear()
{
    entry_documents_.clear();
    exit_documents_.clear();
    state_documents_.clear();
    event_documents_.clear();
}
