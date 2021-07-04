#ifndef INCLUDED_WORLD
#define INCLUDED_WORLD

#include "iworld.h"
#include "../sagedocuments/cellmapdocument.h"

class World
    : public IWorld
{
public:
    World(GameDocument* game_doc);
    ~World();

    void SetContentDir(const std::string& content_dir) { content_dir_ = content_dir; }

    // IWorld
    void TimeDelta(float time_delta)                            { time_delta_ = time_delta; }
    float TimeDelta() const                                     { return time_delta_; }
    const std::string& GetContentDir() const                    { return content_dir_; }
    GameDocument* GetGameDocument() const                       { return game_doc_; }
    CellMapDocument* GetCellMap() const                         { return (CellMapDocument*)&cell_map_; }
    Util::ResourceContextMap* GetResourceContextMap() const     { return (Util::ResourceContextMap*)&res_cxt_map_; }
    Util::ResourceContext* GetResourceContext(const std::string& name) const;
    CameraMap* GetCameraMap() const                             { return (CameraMap*)&camera_map_; }
    Camera* CurrentCamera() const                               { return camera_; }
    bool CurrentCamera(const std::string& name);
    void CurrentCamera(Camera* camera)                          { camera_ = camera; }
    bool CameraCollision() const                                { return camera_collision_; }
    void CameraCollision(bool collision)                        { camera_collision_ = collision; }

private:
    float                       time_delta_;
    GameDocument*               game_doc_;
    CellMapDocument             cell_map_;
    Util::ResourceContextMap    res_cxt_map_;
    CameraMap                   camera_map_;
    Camera*                     camera_;
    std::string                 content_dir_;
    bool                        camera_collision_;
};

#endif  // INCLUDED_WORLD
