#ifndef INCLUDED_COMMAND
#define INCLUDED_COMMAND

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <string>
#include <vector>

struct IWorld;
class CommandInvokeContext;
class Command;

typedef boost::shared_ptr<Command> CommandPtr;
typedef boost::shared_ptr<CommandInvokeContext> CommandInvokeContextPtr;


class CommandInvokeContext
{
public:
    CommandInvokeContext(CommandPtr command)
        : command_(command) {}

    virtual void Execute() {}
    virtual void Execute(float time_delta) {}

protected:
    CommandPtr GetCommand() const { return command_; }

private:
    CommandPtr command_;
};


class Command
    : public boost::enable_shared_from_this<Command>
{
public:
    Command(IWorld* world)
        : world_(world) {}

    virtual CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line) = 0;

    IWorld* GetWorld() const { return world_; }

private:
    IWorld* world_;
};

#endif  // INCLUDED_COMMAND
