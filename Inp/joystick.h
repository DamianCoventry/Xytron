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
#ifndef INCLUDED_INP_JOYSTICK
#define INCLUDED_INP_JOYSTICK

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

///////////////////////////////////////////////////////////////////////////////
namespace Inp
{

///////////////////////////////////////////////////////////////////////////////
class Joystick
{
public:
    Joystick();
    ~Joystick();

    void Initialise(IDirectInput8* dinput, HWND window, const GUID& guid);
    void Shutdown();

    void Activate();
    void Deactivate();

    void FrameBegin();
    void FrameEnd();

    bool IsButtonPressed(int button);
    bool IsButtonReleased(int button);
    bool IsButtonHeld(int button);

    enum Direction { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN };
    bool IsPovButtonPressed(Direction dir, int pov = 0);
    bool IsPovButtonReleased(Direction dir, int pov = 0);
    bool IsPovButtonHeld(Direction dir, int pov = 0);

    void GetMovement(int* x_delta, int* y_delta, int* z_delta);
    void GetRotation(int* x_delta, int* y_delta, int* z_delta);

    int GetSliderPos(int slider);

    enum CONSTANTS
    { RANGE = 1000, NUM_BUTTONS = 32, NUM_SLIDERS = 2, NUM_POVS = 4 };

private:
    IDirectInput8*          dinput_;
    IDirectInputDevice8*    device_;
    DIJOYSTATE              state_;
    DIJOYSTATE              old_state_;
    GUID                    guid_;
};

}       // namespace Inp

#endif  // INCLUDED_INP_JOYSTICK
