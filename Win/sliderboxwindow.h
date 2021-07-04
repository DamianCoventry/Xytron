#ifndef INCLUDED_WIN_SLIDERBOXWINDOW
#define INCLUDED_WIN_SLIDERBOXWINDOW

#include "window.h"
#include <boost/function.hpp>

namespace Win
{

class SliderBoxWindow
    : public Window
{
public:
    typedef boost::function<void (float)> OnSliderBoxMovedHandler;

public:
    SliderBoxWindow(WindowManager* manager, Window* parent = NULL)
        : Window(manager, parent), min_x_(0.0f), max_x_(0.0f) {}

    void MinX(float min_x) { min_x_ = min_x; }
    void MaxX(float max_x) { max_x_ = max_x; }

    float MinX() const { return min_x_; }
    float MaxX() const { return max_x_; }

    void Enabled(bool enabled);

    bool OnMouseMoved(int x_delta, int y_delta);

    void OnSliderBoxMoved(OnSliderBoxMovedHandler slider_box_moved_handler)
    { slider_box_moved_handler_ = slider_box_moved_handler; }

private:
    float min_x_;
    float max_x_;
    OnSliderBoxMovedHandler slider_box_moved_handler_;
};

typedef boost::shared_ptr<SliderBoxWindow> SliderBoxWindowPtr;

}       // namespace Win

#endif  // INCLUDED_WIN_SLIDERBOXWINDOW
