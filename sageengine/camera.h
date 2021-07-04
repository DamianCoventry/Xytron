#ifndef INCLUDED_CAMERA
#define INCLUDED_CAMERA

#include "../Math/vector.h"
#include <string>
#include <map>

class Camera
{
public:
    void MoveForward(float time_delta);
    void MoveBackward(float time_delta);
    void MoveLeft(float time_delta);
    void MoveRight(float time_delta);
    void MoveUp(float time_delta);
    void MoveDown(float time_delta);

    Math::Vector GetMoveForward(float time_delta);
    Math::Vector GetMoveBackward(float time_delta);
    Math::Vector GetMoveLeft(float time_delta);
    Math::Vector GetMoveRight(float time_delta);
    Math::Vector GetMoveUp(float time_delta);
    Math::Vector GetMoveDown(float time_delta);

    void UpdateRotation(int x_delta, int y_delta);

    void Position(const Math::Vector& position)     { position_ = position; }
    void Rotation(const Math::Vector& rotation)     { rotation_ = rotation; }

    const Math::Vector& Position() const            { return position_; }
    const Math::Vector& Rotation() const            { return rotation_; }

private:
    Math::Vector position_;
    Math::Vector rotation_;
};

typedef std::map<std::string, Camera> CameraMap;

#endif  // INCLUDED_CAMERA
