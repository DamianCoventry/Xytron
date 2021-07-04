#include "commandlineset.h"
#include <algorithm>
#include <boost/bind.hpp>

void CommandLineSet::Execute()
{
    std::for_each(
        command_invocations_.begin(),
        command_invocations_.end(),
        boost::bind(&CommandInvokeContext::Execute, _1));
}

void CommandLineSet::Execute(float time_delta)
{
    std::for_each(
        command_invocations_.begin(),
        command_invocations_.end(),
        boost::bind(&CommandInvokeContext::Execute, _1, time_delta));
}
