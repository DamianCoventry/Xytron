#ifndef INCLUDED_WIN_MOUSECURSOR
#define INCLUDED_WIN_MOUSECURSOR

#include "../gfx/gfx.h"
#include "../util/resourcecontext.h"

namespace Win
{

class MouseCursor
{
public:
    MouseCursor();

    void Show()             { show_ = true; }
    void Hide()             { show_ = false; }
    bool Visible() const    { return show_; }

    void BindResources(Util::ResourceContext* resources);
    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);
    void UpdatePosition(float x_delta, float y_delta);

    void MinPosition(const Math::Vector& min_position)     { min_position_ = min_position; }
    void MaxPosition(const Math::Vector& max_position)     { max_position_ = max_position; }
    void Position(const Math::Vector& position)            { position_ = position; }

    const Math::Vector& Position() const { return position_; }

private:
    bool                show_;
    Gfx::ImageAnimPtr   imgani_cursor_;
    Math::Vector        position_;
    Math::Vector        min_position_;
    Math::Vector        max_position_;
    float               ani_time_;
};

}       // namespace Win

#endif  // INCLUDED_WIN_MOUSECURSOR
