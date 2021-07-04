#ifndef INCLUDED_COMMANDLINESET
#define INCLUDED_COMMANDLINESET

#include "command.h"
#include <vector>

class CommandLineSet
{
public:
    typedef std::vector<CommandInvokeContextPtr> CommandInvocations;

public:
    CommandInvocations& GetCommandInvocations() const { return (CommandInvocations&)command_invocations_; }

    void Execute();
    void Execute(float time_delta);

public:
    CommandInvocations command_invocations_;
};

#endif  // INCLUDED_COMMANDLINESET
