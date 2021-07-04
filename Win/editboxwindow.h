#ifndef INCLUDED_WIN_EDITBOXWINDOW
#define INCLUDED_WIN_EDITBOXWINDOW

#include "window.h"
#include <boost/function.hpp>

namespace Win
{

class EditBoxWindow
    : public Window
{
public:
    typedef boost::function<void ()> OnEditBoxReturnPressedHander;

public:
    EditBoxWindow(WindowManager* manager, Window* parent = NULL);

    std::string& Text() const       { return (std::string&)text_; }

    void Font(Gfx::FontPtr font)            { font_ = font; }
    void FontColor(const Gfx::Color& color) { color_ = color; }

    void OnEditBoxReturnPressed(OnEditBoxReturnPressedHander return_pressed_handler) { return_pressed_handler_ = return_pressed_handler; }

    void TopMargin(float margin)    { top_margin_ = margin; }
    void HorizMargin(float margin)  { horiz_margin_ = margin; }

    void Enabled(bool enabled);

    void Think(float time_delta);
    void Draw2d(const Gfx::Graphics& g);

    bool OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);

private:
    void UpdateFirstChar();
    void MoveCursorToNextWord();
    void MoveCursorToPrevWord();

private:
    OnEditBoxReturnPressedHander return_pressed_handler_;
    std::string text_;
    unsigned int first_char_;
    std::size_t cursor_index_;
    bool cursor_blink_;
    float cursor_time_;
    float top_margin_;
    float horiz_margin_;
    Gfx::FontPtr font_;
    Gfx::Color color_;
};

typedef boost::shared_ptr<EditBoxWindow> EditBoxWindowPtr;

}       // namespace Win

#endif  // INCLUDED_WIN_EDITBOXWINDOW
