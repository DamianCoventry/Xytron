#include "stdafx.h"
#include "checkboxwindow.h"
#include "windowmanager.h"

using namespace Win;

CheckBoxWindow::CheckBoxWindow(WindowManager* manager, Window* parent)
: Window(manager, parent)
, checked_(false)
{
    Moveable(false);
    Focusable(true);
    Modal(false);
}

void CheckBoxWindow::Enabled(bool enabled)
{
    Window::Enabled(enabled);
    if(enabled)
    {
        CurrentFrame(checked_ ? 3 : 0);
    }
    else
    {
        CurrentFrame(checked_ ? 5 : 2);
    }
}

void CheckBoxWindow::Checked(bool checked)
{
    checked_ = checked;
    CurrentFrame(checked_ ? 3 : 0);
}

bool CheckBoxWindow::OnMouseButtonPressed(int CheckBox, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!Window::Enabled() || !Visible())
    {
        return false;
    }

    bool rv = Window::OnMouseButtonPressed(CheckBox, shift_held, ctrl_held, alt_held, win_held);
    if(rv)
    {
        checked_ = !checked_;
        CurrentFrame(checked_ ? 3 : 0);
    }
    return rv;
}

bool CheckBoxWindow::OnMouseButtonReleased(int CheckBox, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!Window::Enabled() || !Visible())
    {
        return false;
    }

    bool rv = false;
    if(Manager()->MouseCapture() == shared_from_this())
    {
        Math::Vector cursor_pos;
        if(Parent())
        {
            cursor_pos = Parent()->ToClientCoords(Manager()->GetMouseCursor().Position());
        }
        else
        {
            cursor_pos = Manager()->GetMouseCursor().Position();
        }

        if(HitTest(cursor_pos))
        {
            if(on_click_handler_)
            {
                on_click_handler_();
            }
            CurrentFrame(checked_ ? 3 : 0);
            rv = true;
        }

        Manager()->MouseCapture(WindowPtr());
    }

    return rv;
}

bool CheckBoxWindow::OnMouseMoved(int x_delta, int y_delta)
{
    if(!Window::Enabled() || !Visible())
    {
        return false;
    }

    Math::Vector cursor_pos;
    if(Parent())
    {
        cursor_pos = Parent()->ToClientCoords(Manager()->GetMouseCursor().Position());
    }
    else
    {
        cursor_pos = Manager()->GetMouseCursor().Position();
    }

    if(HitTest(cursor_pos))
    {
        CurrentFrame(checked_ ? 4 : 1);
    }
    else
    {
        CurrentFrame(checked_ ? 3 : 0);
    }

    return false;
}

void CheckBoxWindow::OnMouseEnter()
{
    if(!Window::Enabled() || !Visible())
    {
        return;
    }

    CurrentFrame(checked_ ? 4 : 1);
}

void CheckBoxWindow::OnMouseLeave()
{
    if(!Window::Enabled() || !Visible())
    {
        return;
    }

    CurrentFrame(checked_ ? 3 : 0);
}
