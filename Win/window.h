#ifndef INCLUDED_WIN_WINDOW
#define INCLUDED_WIN_WINDOW

#include "../gfx/gfx.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <list>

namespace Win
{

class WindowManager;
class Window;
typedef boost::shared_ptr<Window> WindowPtr;
typedef std::list<WindowPtr> WindowList;

class Window
    : public boost::enable_shared_from_this<Window>
{
public:
    Window(WindowManager* manager, Window* parent = NULL);
    virtual ~Window();

    void Close();

    Window*         Parent() const      { return parent_; }
    WindowList&     Children() const    { return (WindowList&)children_; }

    void Position(const Math::Vector& position)     { position_ = position; }
    const Math::Vector& Position() const            { return position_; }

    void CurrentFrame(int current_frame)        { current_frame_ = current_frame; }
    int CurrentFrame() const                    { return current_frame_; }

    void ClientImage(Gfx::ImageAnimPtr image)       { imgani_client_ = image; }
    void ClientImageShadow(Gfx::ImageAnimPtr image) { imgani_shadow_ = image; }

    Gfx::ImageAnimPtr ClientImage() const       { return imgani_client_; }
    Gfx::ImageAnimPtr ClientImageShadow() const { return imgani_shadow_; }

    void Modal(bool modal)          { modal_ = modal; }
    bool Modal() const              { return modal_; }

    void Moveable(bool moveable)    { moveable_ = moveable; }
    bool Moveable() const           { return moveable_; }

    void Focusable(bool focusable)  { focusable_ = focusable; }
    bool Focusable() const          { return focusable_; }

    void Visible(bool visible)      { visible_ = visible; }
    bool Visible() const            { return visible_; }

    virtual void Enabled(bool enabled)  { enabled_ = enabled; }
    bool Enabled() const                { return enabled_; }

    bool HitTest(const Math::Vector& point_parent_coords) const;

    WindowPtr GetChildUnderCursor() const;

    WindowPtr GetFocus() const      { return focus_; }
    void Focus();
    void FocusFirst();
    void FocusLast();
    void FocusNext();
    void FocusPrev();

    Math::Vector ToClientCoords(const Math::Vector& screen_coords) const;

    virtual void Think(float time_delta);
    virtual void Draw2d(const Gfx::Graphics& g);

    virtual void OnDefaultAcceptKey() {}
    virtual void OnDefaultCancelKey() {}

    virtual void OnWindowActivated() {}
    virtual void OnWindowDeactivated() {}

    virtual bool OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    virtual bool OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    virtual bool OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);

    virtual bool OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    virtual bool OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    virtual bool OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    virtual bool OnMouseMoved(int x_delta, int y_delta);

    virtual void OnMouseEnter() {}
    virtual void OnMouseLeave() {}

    virtual void OnGetCapture() {}
    virtual void OnLoseCapture() {}
    virtual void OnGetFocus() {}
    virtual void OnLoseFocus() {}

protected:
    WindowManager*      Manager() const     { return manager_; }
    const Math::Vector& DragOffset() const  { return drag_offset_; }

private:
    WindowManager*  manager_;
    Window*         parent_;
    WindowList      children_;
    WindowPtr       focus_;
    bool            modal_;
    bool            moveable_;
    bool            focusable_;
    bool            visible_;
    bool            enabled_;
    Gfx::ImageAnimPtr   imgani_client_;
    Gfx::ImageAnimPtr   imgani_shadow_;
    Math::Vector    position_;
    int             current_frame_;
    Math::Vector    drag_offset_;
};

}       // namespace Win

#endif  // INCLUDED_WIN_WINDOW
