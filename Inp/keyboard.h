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
#ifndef INCLUDED_INP_KEYBOARD
#define INCLUDED_INP_KEYBOARD

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

///////////////////////////////////////////////////////////////////////////////
namespace Inp
{

///////////////////////////////////////////////////////////////////////////////
class Keyboard
{
public:
    Keyboard();
    ~Keyboard();

    void Initialise(IDirectInput8* dinput, HWND window);
    void Shutdown();

    void Activate();
    void Deactivate();

    void FrameBegin();
    void FrameEnd();

    bool IsKeyPressed(int key);
    bool IsKeyReleased(int key);
    bool IsKeyHeld(int key);

    enum Constants
    { NUM_KEYS = 256 };

private:
    IDirectInput8*          dinput_;
    IDirectInputDevice8*    device_;
    char                    keys_[NUM_KEYS];
    char                    old_keys_[NUM_KEYS];
};

}       // namespace Inp

#endif  // INCLUDED_INP_KEYBOARD
