#include "loadcellmapcommand.h"
#include "iworld.h"

#include "../sagedocuments/cellmapdocument.h"
#include "../sagedocuments/cellmapfile.h"
#include "../sagedocuments/gamedocument.h"

#include <boost/bind.hpp>
#include <sstream>

void LoadCellMapCommandInvokeContext::Execute()
{
    IWorld* world = GetCommand()->GetWorld();

    std::ostringstream oss;
    oss << "Maps\\" << name_ << ".cellmap";

    Util::ResourceContext* rc = world->GetResourceContext(res_cxt_);
    if(rc == NULL)
    {
        throw std::runtime_error(std::string("Couldn't find the resource context named [") + res_cxt_ + "]");
    }

    GameDocument* gd = world->GetGameDocument();

    rc->CreateDefaultTexture();
    world->GetCellMap()->InitialiseDefaultTextureSet(rc);
    world->GetCellMap()->BuildTextureSetMap(rc, &(gd->TextureSetDocuments()), world->GetContentDir());

    CellMapFile file;
    if(!file.Load(oss.str(), world->GetCellMap(),
        boost::bind(&LoadCellMapCommandInvokeContext::OnCellMapLoadTotal, this, _1),
        boost::bind(&LoadCellMapCommandInvokeContext::OnCellMapLoadStep, this, _1)))
    {
        throw std::runtime_error(std::string("Couldn't load the 2D cell map from the file [") + oss.str() + "]");
    }

    world->GetCellMap()->BindCellsToTextures(
        boost::bind(&LoadCellMapCommandInvokeContext::OnBindTexturesTotal, this, _1),
        boost::bind(&LoadCellMapCommandInvokeContext::OnBindTexturesStep, this, _1));

    world->GetCellMap()->BuildSkyPolygon(rc, world->GetContentDir());

    world->GetCellMap()->BuildCombinedCellBspTrees(boost::bind(&LoadCellMapCommandInvokeContext::OnBuildCombinedCellBspTreesTotal, this, _1),
        boost::bind(&LoadCellMapCommandInvokeContext::OnBuildCombinedCellBspTreesStep, this, _1));

    //world->GetCellMap()->CalculateLight(world->GetCellMap()->GetLightList(), boost::bind(&LoadCellMapCommandInvokeContext::OnCalculatingLightingTotal, this, _1),
    //    boost::bind(&LoadCellMapCommandInvokeContext::OnCalculatingLightingStep, this, _1));
}

void LoadCellMapCommandInvokeContext::OnBindTexturesTotal(int total)
{
}

void LoadCellMapCommandInvokeContext::OnBindTexturesStep(int value)
{
}

void LoadCellMapCommandInvokeContext::OnCellMapLoadTotal(int total)
{
}

void LoadCellMapCommandInvokeContext::OnCellMapLoadStep(int value)
{
}

void LoadCellMapCommandInvokeContext::OnBuildCombinedCellBspTreesTotal(int total)
{
}

void LoadCellMapCommandInvokeContext::OnBuildCombinedCellBspTreesStep(int value)
{
}

void LoadCellMapCommandInvokeContext::OnCalculatingLightingTotal(int total)
{
}

void LoadCellMapCommandInvokeContext::OnCalculatingLightingStep(int value)
{
}





LoadCellMapCommand::LoadCellMapCommand(IWorld* world)
: Command(world)
{
}

CommandInvokeContextPtr LoadCellMapCommand::CreateCommandInvocation(const std::vector<std::string>& command_line)
{
    // command_line[0] will be the name of this command
    // command_line[1] is the map to use
    // command_line[2] is the resource context to bind with

    if(command_line.size() < 2)
    {
        throw std::runtime_error("Insufficient number of command line arguments for LoadCellMapCommand");
    }

    return CommandInvokeContextPtr(new LoadCellMapCommandInvokeContext(shared_from_this(), command_line[1], command_line[2]));
}
