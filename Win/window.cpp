#include "stdafx.h"
#include "window.h"
#include "windowmanager.h"
#include <algorithm>
#include <boost/bind/bind.hpp>

using namespace Win;
using namespace boost::placeholders;

Window::Window(WindowManager* manager, Window* parent)
: manager_(manager)
, parent_(parent)
, modal_(false)
, moveable_(false)
, focusable_(false)
, visible_(true)
, enabled_(true)
, current_frame_(0)
{
}

Window::~Window()
{
}

void Window::Close()
{
    manager_->HideWindow(shared_from_this());
}

bool Window::HitTest(const Math::Vector& point_parent_coords) const
{
    if(imgani_client_ == NULL)
    {
        return false;
    }

    if(point_parent_coords.x_ < position_.x_) return false;
    if(point_parent_coords.x_ > position_.x_+imgani_client_->FrameWidth()) return false;
    if(point_parent_coords.y_ < position_.y_) return false;
    if(point_parent_coords.y_ > position_.y_+imgani_client_->FrameHeight()) return false;

    return true;
}

WindowPtr Window::GetChildUnderCursor() const
{
    Math::Vector client_coords = ToClientCoords(manager_->GetMouseCursor().Position());

    WindowList::const_iterator itor;
    for(itor = children_.begin(); itor != children_.end(); ++itor)
    {
        if((*itor)->Visible() && (*itor)->Enabled() && (*itor)->HitTest(client_coords))
        {
            return *itor;
        }
    }
    return WindowPtr();
}

void Window::Focus()
{
    if(parent_)
    {
        parent_->focus_ = shared_from_this();
    }
}

void Window::FocusFirst()
{
    WindowList::iterator itor;
    for(itor = children_.begin(); itor != children_.end(); ++itor)
    {
        if((*itor)->Focusable())
        {
            if(focus_)
            {
                focus_->OnLoseFocus();
            }
            focus_ = *itor;
            focus_->OnGetFocus();
            break;
        }
    }
}

void Window::FocusLast()
{
    WindowList::reverse_iterator itor;
    for(itor = children_.rbegin(); itor != children_.rend(); ++itor)
    {
        if((*itor)->Focusable())
        {
            if(focus_)
            {
                focus_->OnLoseFocus();
            }
            focus_ = *itor;
            focus_->OnGetFocus();
            break;
        }
    }
}

void Window::FocusNext()
{
    if(focus_ == NULL)
    {
        FocusFirst();
        return;
    }

    WindowList::iterator itor;
    for(itor = children_.begin(); itor != children_.end(); ++itor)
    {
        if(*itor == focus_)
        {
            break;
        }
    }
    if(itor == children_.end())
    {
        FocusFirst();
        return;
    }

    do
    {
        ++itor;
        if(itor == children_.end())
        {
            itor = children_.begin();
        }
        if(*itor == focus_)
        {
            return;
        }
    }
    while(!(*itor)->Focusable());

    focus_->OnLoseFocus();
    focus_ = *itor;
    focus_->OnGetFocus();
}

void Window::FocusPrev()
{
    if(focus_ == NULL)
    {
        FocusFirst();
        return;
    }

    WindowList::reverse_iterator itor;
    for(itor = children_.rbegin(); itor != children_.rend(); ++itor)
    {
        if(*itor == focus_)
        {
            break;
        }
    }
    if(itor == children_.rend())
    {
        FocusFirst();
        return;
    }

    do
    {
        ++itor;
        if(itor == children_.rend())
        {
            itor = children_.rbegin();
        }
        if(*itor == focus_)
        {
            return;
        }
    }
    while(!(*itor)->Focusable());

    focus_->OnLoseFocus();
    focus_ = *itor;
    focus_->OnGetFocus();
}

Math::Vector Window::ToClientCoords(const Math::Vector& screen_coords) const
{
    Math::Vector offset(position_);

    Window* current = parent_;
    while(current)
    {
        offset += current->position_;
        current = current->parent_;
    }

    return screen_coords - offset;
}

void Window::Think(float time_delta)
{
    if(enabled_)
    {
        std::for_each(children_.begin(), children_.end(), boost::bind(&Window::Think, _1, time_delta));
    }
}

void Window::Draw2d(const Gfx::Graphics& g)
{
    if(!visible_)
    {
        return;
    }

    Math::Vector offset(position_);

    Window* current = parent_;
    while(current)
    {
        offset += current->position_;
        current = current->parent_;
    }

    if(imgani_shadow_)
    {
        imgani_shadow_->Draw2d(g, offset+Math::Vector(16.0f, 16.0f, 0.0f));
    }

    imgani_client_->CurrentFrame(current_frame_);
    imgani_client_->Draw2d(g, offset);

    std::for_each(children_.begin(), children_.end(), boost::bind(&Window::Draw2d, _1, boost::cref(g)));
}

bool Window::OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!enabled_ || !visible_)
    {
        return false;
    }

    if(focus_)
    {
        return focus_->OnKeyPressed(key, shift_held, ctrl_held, alt_held, win_held);
    }
    return false;
}

bool Window::OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!enabled_ || !visible_)
    {
        return false;
    }

    if(focus_)
    {
        return focus_->OnKeyHeld(key, shift_held, ctrl_held, alt_held, win_held);
    }
    return false;
}

bool Window::OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!enabled_ || !visible_)
    {
        return false;
    }

    if(focus_)
    {
        return focus_->OnKeyReleased(key, shift_held, ctrl_held, alt_held, win_held);
    }
    return false;
}

bool Window::OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!enabled_ || !visible_)
    {
        return false;
    }

    Math::Vector cursor_pos = ToClientCoords(manager_->GetMouseCursor().Position());
    if(cursor_pos.x_ >= 0 && cursor_pos.x_ < imgani_client_->FrameWidth())
    {
        WindowPtr child = GetChildUnderCursor();
        if(child)
        {
            child->Focus();
            child->OnMouseButtonPressed(button, shift_held, ctrl_held, alt_held, win_held);
            return true;
        }

        Focus();
        manager_->MouseCapture(shared_from_this());
        drag_offset_ = cursor_pos;
        return true;
    }
    return false;
}

bool Window::OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!enabled_ || !visible_)
    {
        return false;
    }

    WindowPtr child = GetChildUnderCursor();
    if(child)
    {
        child->OnMouseButtonHeld(button, shift_held, ctrl_held, alt_held, win_held);
        return true;
    }

    if(manager_->MouseCapture() == shared_from_this())
    {
        return true;
    }
    return false;
}

bool Window::OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!enabled_ || !visible_)
    {
        return false;
    }

    WindowPtr child = GetChildUnderCursor();
    if(child)
    {
        child->OnMouseButtonReleased(button, shift_held, ctrl_held, alt_held, win_held);
        return true;
    }

    if(manager_->MouseCapture() == shared_from_this())
    {
        manager_->MouseCapture(WindowPtr());
        return true;
    }
    return false;
}

bool Window::OnMouseMoved(int x_delta, int y_delta)
{
    if(!enabled_ || !visible_)
    {
        return false;
    }

    WindowPtr child = GetChildUnderCursor();
    if(child)
    {
        child->OnMouseMoved(x_delta, y_delta);
        return true;
    }

    if(manager_->MouseCapture() == shared_from_this() && moveable_)
    {
        Math::Vector cursor_pos;
        if(parent_)
        {
            cursor_pos = parent_->ToClientCoords(manager_->GetMouseCursor().Position());
        }
        else
        {
            cursor_pos = manager_->GetMouseCursor().Position();
        }

        Position(cursor_pos - drag_offset_);
        return true;
    }
    return false;
}
