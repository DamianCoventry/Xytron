#include "stdafx.h"
#include "windowmanager.h"
#include "window.h"
#include <algorithm>
#include <boost/bind/bind.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

using namespace Win;
using namespace boost::placeholders;

WindowManager::WindowManager()
{
    mouse_cursor_.MinPosition(Math::Vector(0.0f, 0.0f, 0.0f));
    mouse_cursor_.MaxPosition(Math::Vector(800.0f, 600.0f, 0.0f));
}

void WindowManager::ShowWindowTopMost(WindowPtr window)
{
    WindowList::iterator itor = std::find(windows_.begin(), windows_.end(), window);
    if(itor != windows_.end())
    {
        windows_.erase(itor);
    }
    if(!windows_.empty())
    {
        windows_.front()->OnWindowDeactivated();
    }
    windows_.push_front(window);
    window->OnWindowActivated();
}

void WindowManager::ShowWindowBottomMost(WindowPtr window)
{
    WindowList::iterator itor = std::find(windows_.begin(), windows_.end(), window);
    if(itor != windows_.end())
    {
        windows_.erase(itor);
    }
    windows_.push_back(window);
}

void WindowManager::HideWindow(WindowPtr window)
{
    WindowList::iterator itor = std::find(windows_.begin(), windows_.end(), window);
    if(itor != windows_.end())
    {
        windows_.erase(itor);
    }
    if(!windows_.empty())
    {
        windows_.front()->OnWindowActivated();
    }
}

void WindowManager::HideAllWindows()
{
    std::for_each(windows_.begin(), windows_.end(), boost::bind(&Window::OnWindowDeactivated, _1));
    windows_.clear();
}

void WindowManager::MouseCapture(WindowPtr window)
{
    if(mouse_capture_)
    {
        mouse_capture_->OnLoseCapture();
    }
    mouse_capture_ = window;
    if(mouse_capture_)
    {
        mouse_capture_->OnGetCapture();
    }
}

void WindowManager::Think(float time_delta)
{
    mouse_cursor_.Think(time_delta);
    std::for_each(windows_.begin(), windows_.end(), boost::bind(&Window::Think, _1, time_delta));
}

void WindowManager::Draw2d(const Gfx::Graphics& g)
{
    // Draw in the reverse order
    //std::for_each(windows_.begin(), windows_.end(), boost::bind(&Window::Draw2d, _1, boost::cref(g)));
    WindowList::reverse_iterator itor;
    for(itor = windows_.rbegin(); itor != windows_.rend(); ++itor)
    {
        (*itor)->Draw2d(g);
    }

    if(mouse_cursor_.Visible())
    {
        mouse_cursor_.Draw2d(g);
    }
}

void WindowManager::OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(windows_.empty())
    {
        return;
    }
    WindowPtr top_most = windows_.front();
    if(!top_most->OnKeyPressed(key, shift_held, ctrl_held, alt_held, win_held))
    {
        switch(key)
        {
        case DIK_TAB:
            shift_held ? top_most->FocusPrev() : top_most->FocusNext();
            break;
        case DIK_HOME:
            top_most->FocusFirst();
            break;
        case DIK_END:
            top_most->FocusLast();
            break;
        case DIK_RETURN:
        case DIK_NUMPADENTER:
            top_most->OnDefaultAcceptKey();
            break;
        case DIK_ESCAPE:
            top_most->OnDefaultCancelKey();
            break;
        }
    }
}

void WindowManager::OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(windows_.empty())
    {
        return;
    }
    windows_.front()->OnKeyHeld(key, shift_held, ctrl_held, alt_held, win_held);
}

void WindowManager::OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(windows_.empty())
    {
        return;
    }
    windows_.front()->OnKeyReleased(key, shift_held, ctrl_held, alt_held, win_held);
}

void WindowManager::OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(windows_.empty() || !mouse_cursor_.Visible())
    {
        return;
    }

    if(mouse_capture_)
    {
        mouse_capture_->OnMouseButtonPressed(button, shift_held, ctrl_held, alt_held, win_held);
        return;
    }

    WindowPtr top_most = windows_.front();
    if(top_most->Modal())
    {
        if(top_most->HitTest(mouse_cursor_.Position()))
        {
            top_most->OnMouseButtonPressed(button, shift_held, ctrl_held, alt_held, win_held);
        }
        return;
    }

    WindowList::iterator itor;
    for(itor = windows_.begin(); itor != windows_.end(); ++itor)
    {
        if((*itor)->Visible() && (*itor)->Enabled())
        {
            if((*itor)->HitTest(mouse_cursor_.Position()))
            {
                WindowPtr window = *itor;
                ShowWindowTopMost(window);

                window->OnMouseButtonPressed(button, shift_held, ctrl_held, alt_held, win_held);
                return;
            }
        }
    }
}

void WindowManager::OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(windows_.empty() || !mouse_cursor_.Visible())
    {
        return;
    }

    if(mouse_capture_)
    {
        mouse_capture_->OnMouseButtonHeld(button, shift_held, ctrl_held, alt_held, win_held);
        return;
    }
}

void WindowManager::OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(windows_.empty() || !mouse_cursor_.Visible())
    {
        return;
    }

    if(mouse_capture_)
    {
        mouse_capture_->OnMouseButtonReleased(button, shift_held, ctrl_held, alt_held, win_held);
        return;
    }
}

void WindowManager::OnMouseMoved(int x_delta, int y_delta)
{
    WindowPtr old_window_under_cursor;
    WindowList::iterator itor;
    for(itor = windows_.begin(); itor != windows_.end(); ++itor)
    {
        if((*itor)->HitTest(mouse_cursor_.Position()))
        {
            WindowPtr child = (*itor)->GetChildUnderCursor();
            old_window_under_cursor = (child ? child : *itor);
            break;
        }
    }

    mouse_cursor_.UpdatePosition(float(x_delta), float(y_delta));
    if(windows_.empty())
    {
        return;
    }

    if(mouse_capture_)
    {
        mouse_capture_->OnMouseMoved(x_delta, y_delta);
        return;
    }

    WindowPtr new_window_under_cursor;
    for(itor = windows_.begin(); itor != windows_.end(); ++itor)
    {
        if((*itor)->HitTest(mouse_cursor_.Position()))
        {
            WindowPtr child = (*itor)->GetChildUnderCursor();
            new_window_under_cursor = (child ? child : *itor);
            break;
        }
    }

    if(old_window_under_cursor != new_window_under_cursor)
    {
        if(old_window_under_cursor)
        {
            old_window_under_cursor->OnMouseLeave();
        }
        if(new_window_under_cursor)
        {
            new_window_under_cursor->OnMouseEnter();
        }
    }
    if(new_window_under_cursor)
    {
        new_window_under_cursor->OnMouseMoved(x_delta, y_delta);
    }
}
