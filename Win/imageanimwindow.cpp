#include "stdafx.h"
#include "imageanimwindow.h"

using namespace Win;

void ImageAnimWindow::Think(float time_delta)
{
    if(!Window::Enabled())
    {
        return;
    }

    anim_time_ += time_delta;
    if(anim_time_ >= anim_threshold_)
    {
        anim_time_ = 0.0f;

        int frame = CurrentFrame();
        if(++frame >= ClientImage()->ImageCount())
        {
            frame = 0;
        }
        CurrentFrame(frame);
    }
}

void ImageAnimWindow::Fps(unsigned int fps)
{
    fps_ = fps;
    anim_threshold_ = (1000.0f / float(fps_)) / 1000.0f;
}
