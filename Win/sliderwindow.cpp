#include "stdafx.h"
#include "sliderwindow.h"
#include "windowmanager.h"
#include <boost/bind/bind.hpp>

using namespace Win;
using namespace boost::placeholders;

SliderWindow::SliderWindow(WindowManager* manager, Window* parent)
: Window(manager, parent)
, value_(50)
, max_value_(100)
, min_value_(0)
{
    Moveable(false);
    Focusable(true);
    Modal(false);

    box_.reset(new SliderBoxWindow(manager, this));
    box_->Moveable(true);
    box_->Focusable(true);
    box_->OnSliderBoxMoved(boost::bind(&SliderWindow::OnSliderBoxMoved, this, _1));
    Children().push_back(box_);
}

void SliderWindow::Enabled(bool enabled)
{
    Window::Enabled(enabled);
    CurrentFrame(enabled ? 0 : 1);
    box_->Enabled(enabled);
}

void SliderWindow::LeftMargin(float left_margin)
{
    box_->MinX(left_margin);
    if(box_->Position().x_ < left_margin)
    {
        box_->Position(Math::Vector(left_margin, box_->Position().y_, 0.0f));
    }
}

void SliderWindow::RightMargin(float right_margin)
{
    if(ClientImage())
    {
        box_->MaxX(ClientImage()->FrameWidth() - right_margin);
        if(box_->Position().x_ > (ClientImage()->FrameWidth() - right_margin))
        {
            box_->Position(Math::Vector(ClientImage()->FrameWidth() - right_margin, box_->Position().y_, 0.0f));
        }
    }
}

void SliderWindow::VertOffset(float vert_offset)
{
    box_->Position(Math::Vector(box_->Position().x_, vert_offset, 0.0f));
}

void SliderWindow::OnSliderBoxMoved(float x_position)
{
    float range = box_->MaxX() - box_->MinX();
    float value = x_position - box_->MinX();
    float percent = value / range;

    value_ = min_value_ + int(percent * float(max_value_ - min_value_));

    slider_value_changed_handler_(value_);
}

void SliderWindow::UpdateBoxMetrics()
{
    int range = max_value_ - min_value_;
    int value = value_ - min_value_;
    float percent = float(value) / float(range);

    float x_position = box_->MinX() + (percent * (box_->MaxX() - box_->MinX()));

    box_->Position(Math::Vector(x_position, box_->Position().y_, 0.0f));
}
