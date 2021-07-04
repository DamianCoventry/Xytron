#ifndef INCLUDED_WIN_SLIDERWINDOW
#define INCLUDED_WIN_SLIDERWINDOW

#include "window.h"
#include "sliderboxwindow.h"
#include <boost/function.hpp>

namespace Win
{

class SliderWindow
    : public Window
{
public:
    typedef boost::function<void (int)> OnSliderValueChangedHandler;

public:
    SliderWindow(WindowManager* manager, Window* parent = NULL);

    void Value(int value)         { value_ = value; UpdateBoxMetrics(); }
    void MaxValue(int max_value)  { max_value_ = max_value; UpdateBoxMetrics(); }
    void MinValue(int min_value)  { min_value_ = min_value; UpdateBoxMetrics(); }

    int Value() const        { return value_; }
    int MaxValue() const     { return max_value_; }
    int MinValue() const     { return min_value_; }

    void LeftMargin(float left_margin);
    void RightMargin(float right_margin);
    void VertOffset(float vert_offset);

    void Enabled(bool enabled);

    void BoxImage(Gfx::ImageAnimPtr imgani) { box_->ClientImage(imgani); }

    void OnSliderValueChanged(OnSliderValueChangedHandler slider_value_changed_handler)
    { slider_value_changed_handler_ = slider_value_changed_handler; }

private:
    void UpdateBoxMetrics();
    void OnSliderBoxMoved(float x_position);

private:
    OnSliderValueChangedHandler slider_value_changed_handler_;
    int value_;
    int max_value_;
    int min_value_;

    SliderBoxWindowPtr box_;
};

typedef boost::shared_ptr<SliderWindow> SliderWindowPtr;

}       // namespace Win

#endif  // INCLUDED_WIN_SLIDERWINDOW
