#ifndef INCLUDED_COMMANDREPOSITORY
#define INCLUDED_COMMANDREPOSITORY

#include "command.h"
#include <string>
#include <map>

class CommandRepository
{
public:
    void Add(const std::string& name, CommandPtr command);
    CommandPtr Find(const std::string& name) const;
private:
    typedef std::map<std::string, CommandPtr> Commands;
    Commands commands_;
};

#endif  // INCLUDED_COMMANDREPOSITORY
