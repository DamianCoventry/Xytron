#ifndef INCLUDED_WIN_BUTTONWINDOW
#define INCLUDED_WIN_BUTTONWINDOW

#include "window.h"
#include <boost/function.hpp>

namespace Win
{

class ButtonWindow
    : public Window
{
public:
    typedef boost::function<void ()> OnButtonClickHandler;

public:
    ButtonWindow(WindowManager* manager, Window* parent = NULL);

    void OnButtonClick(OnButtonClickHandler on_click_handler) { on_click_handler_ = on_click_handler; }

    void Enabled(bool enabled);

    bool OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    bool OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    bool OnMouseMoved(int x_delta, int y_delta);
    void OnMouseEnter();
    void OnMouseLeave();

private:
    OnButtonClickHandler on_click_handler_;
};

typedef boost::shared_ptr<ButtonWindow> ButtonWindowPtr;

}       // namespace Win

#endif  // INCLUDED_WIN_ButtonWindow
