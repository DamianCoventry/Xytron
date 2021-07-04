#include "stdafx.h"
#include "sliderboxwindow.h"
#include "windowmanager.h"

using namespace Win;

void SliderBoxWindow::Enabled(bool enabled)
{
    Window::Enabled(enabled);
    CurrentFrame(enabled ? 0 : 1);
}

bool SliderBoxWindow::OnMouseMoved(int x_delta, int y_delta)
{
    if(!Window::Enabled() || !Visible())
    {
        return false;
    }

    if(Manager()->MouseCapture() == shared_from_this() && Moveable())
    {
        Math::Vector cursor_pos;
        cursor_pos = Parent()->ToClientCoords(Manager()->GetMouseCursor().Position());

        Math::Vector pos(cursor_pos - DragOffset());
        if(pos.x_ < min_x_) pos.x_ = min_x_;
        else if(pos.x_ > max_x_) pos.x_ = max_x_;
        pos.y_ = Position().y_;
        Position(pos);

        slider_box_moved_handler_(pos.x_);
        return true;
    }
    return false;
}
