///////////////////////////////////////////////////////////////////////////////
//
//  PROJECT: Sage
//
//  AUTHOR: Damian Coventry
//
//  CREATION DATE: June-July 2007
// 
//  COPYRIGHT NOTICE:
//
//      (C) Omenware
//      Created in 2007 as an unpublished copyright work.  All rights reserved.
//      This document and the information it contains is confidential and
//      proprietary to Omenware.  Hence, it may not be  used, copied, reproduced,
//      transmitted, or stored in any form or by any means, electronic,
//      recording, photocopying, mechanical or otherwise, without the prior
//      written permission of Omenware
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#ifndef INCLUDED_INP_INPUT
#define INCLUDED_INP_INPUT

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace Inp
{

///////////////////////////////////////////////////////////////////////////////
struct JoystickInfo
{
    GUID guid_;
    std::string name_;
};

typedef std::vector<JoystickInfo> JoystickDeviceInfoList;



struct InputEvents;

///////////////////////////////////////////////////////////////////////////////
class Input
{
public:
    static char KeyToChar(int dik, bool uppercase);
    static int CharToKey(char c);

public:
    Input();
    ~Input();

    void Initialise(HINSTANCE instance, HWND window, InputEvents* event_handler);
    void Shutdown();

    JoystickDeviceInfoList EnumerateJoysticks();
    void ChooseJoystick(HWND window, const GUID& guid);

    void FrameBegin();
    void FrameEnd();
    void Activate();
    void Deactivate();
    void Poll();

    const Keyboard& GetKeyboard() const     { return keyboard_; }
    const Mouse& GetMouse() const           { return mouse_; }
    const Joystick& GetJoystick() const     { return joystick_; }

private:
    InputEvents*        event_handler_;
    IDirectInput8*      dinput_;
    Keyboard            keyboard_;
    Mouse               mouse_;
    Joystick            joystick_;
    int                 slider_pos_[Joystick::NUM_SLIDERS];
};

}       // namespace Inp

#endif  // INCLUDED_INP_INPUT
