#ifndef INCLUDED_INP_BINDINGS
#define INCLUDED_INP_BINDINGS

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace Inp
{

struct BindingEvents
{
    virtual void OnBindingPressed(){}
    virtual void OnBindingHeld(){}
    virtual void OnBindingReleased(){}
    virtual void OnBindingMovedX(int delta){}
    virtual void OnBindingMovedY(int delta){}
    virtual void OnBindingMoved(int x_delta, int y_delta){}
    virtual void OnBindingMovedUp(int delta){}
    virtual void OnBindingMovedDown(int delta){}
};
typedef boost::shared_ptr<BindingEvents> BindingEventsPtr;

class Bindings
{
public:
    // Use the DirectInput DIK_* values for the keys
    enum MouseBindings
    {
        MSE_MOVE = 256,
        MSE_MOVEX,
        MSE_MOVEY,
        MSE_BUTTON0,
        MSE_BUTTON1,
        MSE_BUTTON2,
        MSE_MWHEELUP,
        MSE_MWHEELDOWN
    };

    static void BuildLookups();
    static std::string ToString(int binding);
    static int ToBinding(const std::string& text);

public:
    Bindings();
    void Set(int binding, BindingEventsPtr handler);
    void Clear(int binding);
    void ClearAll();

    void Activate()     { activated_ = true; }
    void Deactivate()   { activated_ = false; }

    void OnKeyPressed(int key);
    void OnKeyHeld(int key);
    void OnKeyReleased(int key);

    void OnMouseButtonPressed(int button);
    void OnMouseButtonHeld(int button);
    void OnMouseButtonReleased(int button);
    void OnMouseMoved(int x_delta, int y_delta);
    void OnMouseWheelMoved(int delta);

private:
    static std::map<std::string, int> lookups_;

    bool activated_;
    typedef std::vector<BindingEventsPtr> Handlers;
    Handlers handlers_;
};

}       // namespace Inp

#endif  // INCLUDED_INP_BINDINGS
