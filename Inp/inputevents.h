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
#ifndef INCLUDED_INP_INPUTEVENTS
#define INCLUDED_INP_INPUTEVENTS

///////////////////////////////////////////////////////////////////////////////
namespace Inp
{

///////////////////////////////////////////////////////////////////////////////
struct InputEvents
{
    virtual ~InputEvents() {}

    virtual void OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}

    virtual void OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held) {}
    virtual void OnMouseMoved(int x_delta, int y_delta) {}

    enum JoyPovDirection { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN };

    virtual void OnJoyButtonPressed(int button) {}
    virtual void OnJoyButtonHeld(int button) {}
    virtual void OnJoyButtonReleased(int button) {}
    virtual void OnJoyPovButtonPressed(JoyPovDirection dir, int pov) {}
    virtual void OnJoyPovButtonHeld(JoyPovDirection dir, int pov) {}
    virtual void OnJoyPovButtonReleased(JoyPovDirection dir, int pov) {}
    virtual void OnJoyMoved(int x_delta, int y_delta, int z_delta) {}
    virtual void OnJoyRotated(int x_delta, int y_delta, int z_delta) {}
    virtual void OnJoySliderMoved(int slider, int pos) {}
};

}       // namespace Inp

#endif  // INCLUDED_INP_INPUTEVENTS
