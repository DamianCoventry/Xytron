#ifndef INCLUDED_CELLMAPDOCUMENT
#define INCLUDED_CELLMAPDOCUMENT

#include "../sagestudio/worldtypes.h"
#include "texturesetdocument.h"
#include <vector>
#include <boost/function.hpp>
#include "light.h"

namespace Util
{
class ResourceContext;
}

class CellMapDocument
{
public:
    static float DOOR_FRAME_HEIGHT;
    static float DOOR_FRAME_HALF_THICKNESS;
    static float DOOR_HALF_THICKNESS;
    static float SKY_SCHOCHURE_HEIGHT;
    static int LIGHT_MAP_SIZE;

public:
    CellMapDocument();
    ~CellMapDocument();


    void Name(const std::string& name)  { name_ = name; }
    const std::string& Name() const     { return name_; }

    float CellSize() const          { return cell_size_; }
    void CellSize(float cell_size)  { cell_size_ = cell_size; }

    void InsertCell(const CellIndex& index, CellType type, float cell_size, const std::string& ts);
    void InsertCell(const CellIndex& index, const Cell& cell);
    void RemoveCell(const CellIndex& index);
    void ClearAllCells();
    void Clear();

    CellType GetCellType(const CellIndex& index) const;

    void SkyTextureFilename(const std::string& filename) { sky_texture_filename_ = filename; }
    const std::string& SkyTextureFilename() const        { return sky_texture_filename_; }

    void SetTextureSet(const CellIndex& index, const std::string& ts);
    std::string GetTextureSet(const CellIndex& index) const;

    void SetSkyFlag(const CellIndex& index, bool sky);
    bool GetSkyFlag(const CellIndex& index) const;

    CellMap& GetCellMap() const { return (CellMap&)CellMap_; }
    Cell& GetCell(const CellIndex& index) const;

    void InitialiseDefaultTextureSet(Util::ResourceContext* res_cxt);
    void BuildTextureSetMap(Util::ResourceContext* res_cxt, TextureSetDocumentMap* ts_docs, const std::string& content_dir);
    void BindCellsToTextures(boost::function<void (int)> total_function, boost::function<void (int)> step_function);

    void Think(float time_delta);
    void DrawCellsOpaque_Textured();
    void DrawCellsOpaque_Colored();
    void DrawCellsTranslucent_Textured();
    void DrawSky();

    int GetHighestLightNumber() const;

    void BuildSkyPolygon(Util::ResourceContext* res_cxt, const std::string& content_dir);

    void DetermineCellTextures(const CellIndex& index);

    void BuildCombinedCellBspTrees(boost::function<void (int)> total_function, boost::function<void (int)> step_function);
    bool IsLineOfSight(const Math::LineSegment& ls, float plane_shift) const;
    Math::Vector TraceLineSegment(const Math::LineSegment& ls, float plane_shift) const;

    int AmbientLight() const        { return ambient_; }
    void AmbientLight(int ambient)  { ambient_ = ambient; }
    LightList& GetLightList() const { return (LightList&)light_list_; }
    Light* GetLight(const std::string& name) const;

    void CalculateLight(LightList& lights, boost::function<void (int)> total_function, boost::function<void (int)> step_function);
    void CalculateLight(const CellIndex& ci, LightList& lights);

private:
    bool IsSolidSpace(const CellIndex& i) const;
    bool IsSameType(const CellIndex& i, CellType type) const;
    bool IsSky(const CellIndex& i) const;
    void BuildIndividualBspTree(const CellIndex& index);
    void AddCellsPlanesToBsp(const CellContainer::iterator& itor, Bsp::Tree& tree);
    void CalcSkyDimensions();

    void BuildPolygons(const CellIndex& ci);
    void BuildNormalPolygons(const CellContainer::iterator& i);
    void BuildPitPolygons(const CellContainer::iterator& i);
    void BuildLiquidPolygons(const CellContainer::iterator& i);
    void BuildDoorPolygons(const CellContainer::iterator& i);
    void BuildSecretDoorPolygons(const CellContainer::iterator& i);
    void BuildTranslucentDoorPolygons(const CellContainer::iterator& i);
    void BuildTranslucentSmallWallPolygons(const CellContainer::iterator& i);
    void BuildTranslucentLargeWallPolygons(const CellContainer::iterator& i);
    void BuildWallPolygonsForInternalCell(const CellContainer::iterator& i);

    void CalculateLight_i(const CellContainer::iterator& i, LightList& lights);
    void CalculateLight_i(const CellIndex& ci, float ambient, Gfx::PolygonList& pl, const Math::Vector& offset, LightList& lights);

    void SetupPolygonsLightMap(Gfx::Polygon& P);
    Gfx::TexturePtr CreateBlankLightMap() const;

private:
    float cell_size_;
    CellMap CellMap_;
    std::string name_;
    TextureSetMap ts_map_;
    Gfx::TexturePtr BlankLightMap_;

    Gfx::TexturePtr sky_texture_;
    float min_sky_x_, max_sky_x_;
    float min_sky_z_, max_sky_z_;

    std::string default_texture_set_name_;
    std::string sky_texture_filename_;
    TextureSet default_texture_set_;

    LightList light_list_;
    int ambient_;

    struct LitPixel
    {
        unsigned char R_, G_, B_;
    };
    typedef std::vector<LitPixel> LitPixels;
};

typedef std::vector<std::string> CellMapDocumentList;

#endif  // INCLUDED_CellMapDOCUMENT
