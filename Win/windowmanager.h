#ifndef INCLUDED_WIN_WINDOWMANAGER
#define INCLUDED_WIN_WINDOWMANAGER

#include <boost/shared_ptr.hpp>
#include <list>
#include "mousecursor.h"

namespace Gfx
{
class Graphics;
}

namespace Win
{

class Window;
typedef boost::shared_ptr<Window> WindowPtr;
typedef std::list<WindowPtr> WindowList;

class WindowManager
{
public:
    WindowManager();

    void ShowWindowTopMost(WindowPtr window);
    void ShowWindowBottomMost(WindowPtr window);
    void HideWindow(WindowPtr window);
    void HideAllWindows();

    MouseCursor& GetMouseCursor() const { return (MouseCursor&)mouse_cursor_; }

    WindowPtr MouseCapture() const { return mouse_capture_; }
    void MouseCapture(WindowPtr window);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    void OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseMoved(int x_delta, int y_delta);

private:
    WindowList  windows_;
    WindowPtr   mouse_capture_;
    MouseCursor mouse_cursor_;
};

}       // namespace Win

#endif  // INCLUDED_WIN_WINDOWMANAGER
