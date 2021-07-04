#include "world.h"

World::World(GameDocument* game_doc)
: camera_(NULL)
, time_delta_(0.0f)
, game_doc_(game_doc)
, camera_collision_(true)
{
}

World::~World()
{
}

Util::ResourceContext* World::GetResourceContext(const std::string& name) const
{
    Util::ResourceContextMap::const_iterator i = res_cxt_map_.find(name);
    return (i == res_cxt_map_.end() ? NULL : (Util::ResourceContext*)&i->second);
}

bool World::CurrentCamera(const std::string& name)
{
    CameraMap::iterator i = camera_map_.find(name);
    if(i == camera_map_.end())
    {
        return false;
    }
    camera_ = &i->second;
    return true;
}
