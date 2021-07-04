#ifndef INCLUDED_LOADINGSCREEN
#define INCLUDED_LOADINGSCREEN

#include "../gfx/gfx.h"
#include <sstream>
#include <iomanip>

class LoadingScreen
{
public:
    LoadingScreen(Gfx::Graphics& g, Gfx::ImagePtr background, Gfx::FontPtr font)
        : g_(g), background_(background), font_(font) {}

    void Update(int percent)
    {
        background_->Draw2d(g_, Math::Vector());

        std::ostringstream oss;
        oss << std::setw(3) << percent << " %";
        font_->DrawString(Math::Vector(750.0f, 424.0f, 0.0f), Gfx::Color(1.0f, 1.0f, 0.0f), oss.str());

        g_.PageFlip();
    }

    void Release()
    {
        background_.reset();
        font_.reset();
    }

private:
    Gfx::Graphics& g_;
    Gfx::ImagePtr background_;
    Gfx::FontPtr font_;
};

#endif  // INCLUDED_LOADINGSCREEN
