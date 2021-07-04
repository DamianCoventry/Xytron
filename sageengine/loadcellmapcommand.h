#ifndef INCLUDED_LOADCELLMAPCOMMAND
#define INCLUDED_LOADCELLMAPCOMMAND

#include "command.h"

class CellMapDocument;

class LoadCellMapCommandInvokeContext
    : public CommandInvokeContext
{
public:
    LoadCellMapCommandInvokeContext(CommandPtr command, const std::string& name, const std::string& res_cxt)
        : CommandInvokeContext(command), name_(name), res_cxt_(res_cxt) {}

    void Execute();

private:
    void OnBindTexturesTotal(int total);
    void OnBindTexturesStep(int value);
    void OnCellMapLoadTotal(int total);
    void OnCellMapLoadStep(int value);
    void OnBuildCombinedCellBspTreesTotal(int total);
    void OnBuildCombinedCellBspTreesStep(int value);
    void OnCalculatingLightingTotal(int total);
    void OnCalculatingLightingStep(int value);

private:
    CellMapDocument* map_doc_;
    std::string name_;
    std::string res_cxt_;
};

class LoadCellMapCommand
    : public Command
{
public:
    LoadCellMapCommand(IWorld* world);
    CommandInvokeContextPtr CreateCommandInvocation(const std::vector<std::string>& command_line);
};

#endif  // INCLUDED_LOADCELLMAPCOMMAND
