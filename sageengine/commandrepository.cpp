#include "commandrepository.h"

void CommandRepository::Add(const std::string& name, CommandPtr command)
{
    commands_[name] = command;
}

CommandPtr CommandRepository::Find(const std::string& name) const
{
    Commands::const_iterator itor = commands_.find(name);
    return itor == commands_.end() ? CommandPtr() : itor->second;
}
