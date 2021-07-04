#include "stdafx.h"
#include "buttonwindow.h"
#include "windowmanager.h"

using namespace Win;

ButtonWindow::ButtonWindow(WindowManager* manager, Window* parent)
: Window(manager, parent)
{
    Moveable(false);
    Focusable(true);
    Modal(false);
}

void ButtonWindow::Enabled(bool enabled)
{
    Window::Enabled(enabled);
    CurrentFrame(enabled ? 0 : 3);
}

bool ButtonWindow::OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!Window::Enabled() || !Visible())
    {
        return false;
    }

    bool rv = Window::OnMouseButtonPressed(button, shift_held, ctrl_held, alt_held, win_held);
    if(rv)
    {
        CurrentFrame(2);
    }
    return rv;
}

bool ButtonWindow::OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
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
            CurrentFrame(0);
            rv = true;
        }

        Manager()->MouseCapture(WindowPtr());
    }

    return rv;
}

bool ButtonWindow::OnMouseMoved(int x_delta, int y_delta)
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

    CurrentFrame(HitTest(cursor_pos) ? 1 : 0);
    return false;
}

void ButtonWindow::OnMouseEnter()
{
    if(!Window::Enabled() || !Visible())
    {
        return;
    }

    CurrentFrame(1);
}

void ButtonWindow::OnMouseLeave()
{
    if(!Window::Enabled() || !Visible())
    {
        return;
    }

    CurrentFrame(0);
}
