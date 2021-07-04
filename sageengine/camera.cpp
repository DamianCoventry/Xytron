#include "camera.h"

void Camera::MoveForward(float time_delta)
{
    float x_rot = rotation_.x_*Math::DTOR;
    float y_rot = rotation_.y_*Math::DTOR;
    Math::Vector move(cos(x_rot) * sin(y_rot), -sin(x_rot), cos(x_rot) * cos(y_rot));
    position_ -= move * (time_delta * 250.0f);
}

void Camera::MoveBackward(float time_delta)
{
    float x_rot = rotation_.x_*Math::DTOR;
    float y_rot = rotation_.y_*Math::DTOR;
    Math::Vector move(cos(x_rot) * sin(y_rot), -sin(x_rot), cos(x_rot) * cos(y_rot));
    position_ += move * (time_delta * 250.0f);
}

void Camera::MoveLeft(float time_delta)
{
    float y_rot = (rotation_.y_-90.0f)*Math::DTOR;
    Math::Vector move(sin(y_rot), 0.0f, cos(y_rot));
    position_ += move * (time_delta * 250.0f);
}

void Camera::MoveRight(float time_delta)
{
    float y_rot = (rotation_.y_+90.0f)*Math::DTOR;
    Math::Vector move(sin(y_rot), 0.0f, cos(y_rot));
    position_ += move * (time_delta * 250.0f);
}

void Camera::MoveUp(float time_delta)
{
    Math::Vector move(0.0f, 1.0f, 0.0f);
    position_ += move * (time_delta * 250.0f);
}

void Camera::MoveDown(float time_delta)
{
    Math::Vector move(0.0f, -1.0f, 0.0f);
    position_ += move * (time_delta * 250.0f);
}

Math::Vector Camera::GetMoveForward(float time_delta)
{
    float x_rot = rotation_.x_*Math::DTOR;
    float y_rot = rotation_.y_*Math::DTOR;
    Math::Vector move(cos(x_rot) * sin(y_rot), -sin(x_rot), cos(x_rot) * cos(y_rot));
    return position_ - move * (time_delta * 250.0f);
}

Math::Vector Camera::GetMoveBackward(float time_delta)
{
    float x_rot = rotation_.x_*Math::DTOR;
    float y_rot = rotation_.y_*Math::DTOR;
    Math::Vector move(cos(x_rot) * sin(y_rot), -sin(x_rot), cos(x_rot) * cos(y_rot));
    return position_ + move * (time_delta * 250.0f);
}

Math::Vector Camera::GetMoveLeft(float time_delta)
{
    float y_rot = (rotation_.y_-90.0f)*Math::DTOR;
    Math::Vector move(sin(y_rot), 0.0f, cos(y_rot));
    return position_ + move * (time_delta * 250.0f);
}

Math::Vector Camera::GetMoveRight(float time_delta)
{
    float y_rot = (rotation_.y_+90.0f)*Math::DTOR;
    Math::Vector move(sin(y_rot), 0.0f, cos(y_rot));
    return position_ + move * (time_delta * 250.0f);
}

Math::Vector Camera::GetMoveUp(float time_delta)
{
    Math::Vector move(0.0f, 1.0f, 0.0f);
    return position_ + move * (time_delta * 250.0f);
}

Math::Vector Camera::GetMoveDown(float time_delta)
{
    Math::Vector move(0.0f, -1.0f, 0.0f);
    return position_ + move * (time_delta * 250.0f);
}

void Camera::UpdateRotation(int x_delta, int y_delta)
{
    rotation_.x_ -= float(y_delta);
    rotation_.y_ -= float(x_delta);
}
