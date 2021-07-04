#ifndef INCLUDED_WIN_CHECKBOXWINDOW
#define INCLUDED_WIN_CHECKBOXWINDOW

#include "window.h"
#include <boost/function.hpp>

namespace Win
{

class CheckBoxWindow
    : public Window
{
public:
    typedef boost::function<void ()> OnCheckBoxClickHandler;

public:
    CheckBoxWindow(WindowManager* manager, Window* parent = NULL);

    bool Checked() const        { return checked_; }
    void Checked(bool checked);

    void Enabled(bool enabled);

    void OnCheckBoxClick(OnCheckBoxClickHandler on_click_handler) { on_click_handler_ = on_click_handler; }

    bool OnMouseButtonPressed(int CheckBox, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    bool OnMouseButtonReleased(int CheckBox, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    bool OnMouseMoved(int x_delta, int y_delta);
    void OnMouseEnter();
    void OnMouseLeave();

private:
    OnCheckBoxClickHandler on_click_handler_;
    bool checked_;
};

typedef boost::shared_ptr<CheckBoxWindow> CheckBoxWindowPtr;

}       // namespace Win

#endif  // INCLUDED_WIN_CHECKBOXWINDOW
