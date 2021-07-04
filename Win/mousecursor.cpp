#include "stdafx.h"
#include "mousecursor.h"

using namespace Win;

MouseCursor::MouseCursor()
: ani_time_(0.0f)
, show_(false)
{
}

void MouseCursor::BindResources(Util::ResourceContext* resources)
{
    imgani_cursor_ = resources->FindImageAnim("Images/MouseCursor.tga");
    imgani_cursor_->DrawCentered(false);
}

void MouseCursor::Think(float time_delta)
{
    ani_time_ += time_delta;
    if(ani_time_ >= 0.050f)     // 20fps
    {
        ani_time_ = 0.0f;
        imgani_cursor_->IncCurrentFrame();
    }
}

void MouseCursor::Draw2d(const Gfx::Graphics& g)
{
    imgani_cursor_->Draw2d(g, position_);
}

void MouseCursor::UpdatePosition(float x_delta, float y_delta)
{
    position_.x_ += x_delta;
    if(position_.x_ < min_position_.x_)
    {
        position_.x_ = min_position_.x_;
    }
    else if(position_.x_ > max_position_.x_)
    {
        position_.x_ = max_position_.x_;
    }

    position_.y_ += y_delta;
    if(position_.y_ < min_position_.y_)
    {
        position_.y_ = min_position_.y_;
    }
    else if(position_.y_ > max_position_.y_)
    {
        position_.y_ = max_position_.y_;
    }
}
