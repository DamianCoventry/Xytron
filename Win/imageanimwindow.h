#ifndef INCLUDED_WIN_IMAGEANIMWINDOW
#define INCLUDED_WIN_IMAGEANIMWINDOW

#include "window.h"

namespace Win
{

class ImageAnimWindow
    : public Window
{
public:
    ImageAnimWindow(WindowManager* manager, Window* parent = NULL)
        : Window(manager, parent), anim_time_(0.0f) { Fps(30); }

    void Think(float time_delta);

    void Fps(unsigned int fps);
    unsigned int Fps() const { return fps_; }

private:
    float anim_time_;
    float anim_threshold_;
    unsigned int fps_;
};

typedef boost::shared_ptr<ImageAnimWindow> ImageAnimWindowPtr;

}       // namespace Win

#endif  // INCLUDED_WIN_IMAGEANIMWINDOW
