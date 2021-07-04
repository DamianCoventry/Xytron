#ifndef INCLUDED_IWORLD
#define INCLUDED_IWORLD

#include "../util/resourcecontext.h"
#include "camera.h"

class GameDocument;
class CellMapDocument;


struct IWorld
{
    virtual void TimeDelta(float time_delta) = 0;
    virtual float TimeDelta() const = 0;
    virtual const std::string& GetContentDir() const = 0;
    virtual GameDocument* GetGameDocument() const = 0;
    virtual CellMapDocument* GetCellMap() const = 0;
    virtual Util::ResourceContextMap* GetResourceContextMap() const = 0;
    virtual Util::ResourceContext* GetResourceContext(const std::string& name) const = 0;
    virtual CameraMap* GetCameraMap() const = 0;
    virtual Camera* CurrentCamera() const = 0;
    virtual bool CurrentCamera(const std::string& name) = 0;
    virtual void CurrentCamera(Camera* camera) = 0;
    virtual bool CameraCollision() const = 0;
    virtual void CameraCollision(bool collision) = 0;
};

#endif  // INCLUDED_IWORLD
