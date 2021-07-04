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
#include "stdafx.h"
#include "input.h"
#include "mouse.h"
#include "keyboard.h"
#include "inputevents.h"

#include "../util/guidutil.h"
#include "../util/throw.h"

using namespace Inp;

///////////////////////////////////////////////////////////////////////////////
namespace
{
    BOOL PASCAL DIEnumDevicesCallback(const DIDEVICEINSTANCE* device, void* param)
    {
        if(device && param)
        {
            LOG("Found the DirectInput joystick device [" << device->tszInstanceName << "][" << device->tszProductName << "]");

            std::string prod_str, inst_str;
            Util::GuidToString(device->guidProduct, &prod_str);
            Util::GuidToString(device->guidInstance, &inst_str);

            JoystickInfo info;
            memcpy(&info.guid_, &device->guidInstance, sizeof(GUID));
            info.name_ = device->tszProductName;

            JoystickDeviceInfoList* joysticks = reinterpret_cast<JoystickDeviceInfoList*>(param);
            joysticks->push_back(info);
        }

        return DIENUM_CONTINUE;
    }
}

///////////////////////////////////////////////////////////////////////////////
JoystickDeviceInfoList Input::EnumerateJoysticks()
{
    JoystickDeviceInfoList joysticks;

    LOG("Enumerating available DirectInput joystick devices");

    dinput_->EnumDevices(
        DI8DEVCLASS_GAMECTRL,
        DIEnumDevicesCallback,
        reinterpret_cast<void*>(&joysticks),
        DIEDFL_ATTACHEDONLY);

    return joysticks;
}




///////////////////////////////////////////////////////////////////////////////
Input::Input()
: dinput_(NULL)
, event_handler_(NULL)
{
    for(int i = 0; i < Joystick::NUM_SLIDERS; i++)
    {
        slider_pos_[i] = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
Input::~Input()
{
    Shutdown();
}

///////////////////////////////////////////////////////////////////////////////
void Input::Initialise(HINSTANCE instance, HWND window, InputEvents* event_handler)
{
    LOG("Initialising DirectInput");

    event_handler_ = event_handler;

    HRESULT hr = CoCreateInstance(CLSID_DirectInput8, NULL, CLSCTX_INPROC, IID_IDirectInput8,
        reinterpret_cast<void**>(&dinput_));
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of CLSID_IDirectInput8")
    }

    hr = dinput_->Initialize(instance, DIRECTINPUT_VERSION);
    if(FAILED(hr))
    {
        THROW_COM("Could not Initialize an instance of CLSID_IDirectInput8")
    }

    keyboard_.Initialise(dinput_, window);
    mouse_.Initialise(dinput_, window);

    LOG("DirectInput initialised OK");
}

///////////////////////////////////////////////////////////////////////////////
void Input::Shutdown()
{
    if(dinput_)
    {
        LOG("Shutting down DirectInput");

        keyboard_.Shutdown();
        mouse_.Shutdown();
        joystick_.Shutdown();

        dinput_->Release();
        dinput_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Input::FrameBegin()
{
    keyboard_.FrameBegin();
    mouse_.FrameBegin();
    joystick_.FrameBegin();
}

///////////////////////////////////////////////////////////////////////////////
void Input::FrameEnd()
{
    keyboard_.FrameEnd();
    mouse_.FrameEnd();
    joystick_.FrameEnd();
}

///////////////////////////////////////////////////////////////////////////////
void Input::ChooseJoystick(HWND window, const GUID& guid)
{
    std::string guid_str;
    Util::GuidToString(guid, &guid_str);
    LOG("Choosing joystick [" << guid_str << "]");

    joystick_.Initialise(dinput_, window, guid);
}

///////////////////////////////////////////////////////////////////////////////
void Input::Activate()
{
    LOG("Activating DirectInput devices");
    keyboard_.Activate();
    mouse_.Activate();
    joystick_.Activate();
}

///////////////////////////////////////////////////////////////////////////////
void Input::Deactivate()
{
    LOG("Deactivating DirectInput devices");
    keyboard_.Deactivate();
    mouse_.Deactivate();
    joystick_.Deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void Input::Poll()
{
    int i, x, y, z;

    // Keyboard.
    bool shift_held = (keyboard_.IsKeyHeld(DIK_LSHIFT)     || keyboard_.IsKeyHeld(DIK_RSHIFT));
    bool ctrl_held  = (keyboard_.IsKeyHeld(DIK_LCONTROL)   || keyboard_.IsKeyHeld(DIK_RCONTROL));
    bool alt_held   = (keyboard_.IsKeyHeld(DIK_LMENU)      || keyboard_.IsKeyHeld(DIK_RMENU));
    bool win_held   = (keyboard_.IsKeyHeld(DIK_LWIN)       || keyboard_.IsKeyHeld(DIK_RWIN));
    for(i = 0; i < Keyboard::NUM_KEYS; i++)
    {
        if(keyboard_.IsKeyPressed(i))
        {
            event_handler_->OnKeyPressed(i, shift_held, ctrl_held, alt_held, win_held);
        }
        else if(keyboard_.IsKeyHeld(i))
        {
            event_handler_->OnKeyHeld(i, shift_held, ctrl_held, alt_held, win_held);
        }
        else if(keyboard_.IsKeyReleased(i))
        {
            event_handler_->OnKeyReleased(i, shift_held, ctrl_held, alt_held, win_held);
        }
    }

    // Mouse.
    for(i = 0; i < Mouse::NUM_BUTTONS; i++)
    {
        if(mouse_.IsButtonPressed(i))
        {
            event_handler_->OnMouseButtonPressed(i, shift_held, ctrl_held, alt_held, win_held);
        }
        else if(mouse_.IsButtonHeld(i))
        {
            event_handler_->OnMouseButtonHeld(i, shift_held, ctrl_held, alt_held, win_held);
        }
        else if(mouse_.IsButtonReleased(i))
        {
            event_handler_->OnMouseButtonReleased(i, shift_held, ctrl_held, alt_held, win_held);
        }
    }

    x = y = 0;
    mouse_.GetMovement(&x, &y);
    if(x || y)
    {
        event_handler_->OnMouseMoved(x, y);
    }

    // Joystick.
    for(i = 0; i < Joystick::NUM_BUTTONS; i++)
    {
        if(joystick_.IsButtonPressed(i))
        {
            event_handler_->OnJoyButtonPressed(i);
        }
        else if(joystick_.IsButtonHeld(i))
        {
            event_handler_->OnJoyButtonHeld(i);
        }
        else if(joystick_.IsButtonReleased(i))
        {
            event_handler_->OnJoyButtonReleased(i);
        }
    }

    for(i = 0; i < Joystick::NUM_POVS; i++)
    {
        for(x = (int)Joystick::DIR_LEFT; x <= (int)Joystick::DIR_DOWN; x++)
        {
            if(joystick_.IsPovButtonPressed((Joystick::Direction)x, i))
            {
                event_handler_->OnJoyPovButtonPressed((InputEvents::JoyPovDirection)x, i);
            }
            else if(joystick_.IsPovButtonHeld((Joystick::Direction)x, i))
            {
                event_handler_->OnJoyPovButtonHeld((InputEvents::JoyPovDirection)x, i);
            }
            else if(joystick_.IsPovButtonReleased((Joystick::Direction)x, i))
            {
                event_handler_->OnJoyPovButtonReleased((InputEvents::JoyPovDirection)x, i);
            }
        }
    }

    x = y = z = 0;
    joystick_.GetMovement(&x, &y, &z);
    if(x || y || z)
    {
        event_handler_->OnJoyMoved(x, y, z);
    }

    x = y = z = 0;
    joystick_.GetRotation(&x, &y, &z);
    if(x || y || z)
    {
        event_handler_->OnJoyRotated(x, y, z);
    }

    for(i = 0; i < Joystick::NUM_SLIDERS; i++)
    {
        x = joystick_.GetSliderPos(i);
        if(x != slider_pos_[i])
        {
            event_handler_->OnJoySliderMoved(i, x);
            slider_pos_[i] = x;
        }
    }
}






///////////////////////////////////////////////////////////////////////////////
char Input::KeyToChar(int dik, bool uppercase)
{
    switch(dik)
    {
    case DIK_SPACE: return ' ';
    case DIK_1: return uppercase ? '!' : '1';
    case DIK_2: return uppercase ? '@' : '2';
    case DIK_3: return uppercase ? '#' : '3';
    case DIK_4: return uppercase ? '$' : '4';
    case DIK_5: return uppercase ? '%' : '5';
    case DIK_6: return uppercase ? '^' : '6';
    case DIK_7: return uppercase ? '&' : '7';
    case DIK_8: return uppercase ? '*' : '8';
    case DIK_9: return uppercase ? '(' : '9';
    case DIK_0: return uppercase ? ')' : '0';
    case DIK_MINUS: return uppercase ? '_' : '-';
    case DIK_EQUALS: return uppercase ? '+' : '=';
    case DIK_Q: return uppercase ? 'Q' : 'q';
    case DIK_W: return uppercase ? 'W' : 'w';
    case DIK_E: return uppercase ? 'E' : 'e';
    case DIK_R: return uppercase ? 'R' : 'r';
    case DIK_T: return uppercase ? 'T' : 't';
    case DIK_Y: return uppercase ? 'Y' : 'y';
    case DIK_U: return uppercase ? 'U' : 'u';
    case DIK_I: return uppercase ? 'I' : 'i';
    case DIK_O: return uppercase ? 'O' : 'o';
    case DIK_P: return uppercase ? 'P' : 'p';
    case DIK_LBRACKET: return uppercase ? '{' : '[';
    case DIK_RBRACKET: return uppercase ? '}' : ']';
    case DIK_A: return uppercase ? 'A' : 'a';
    case DIK_S: return uppercase ? 'S' : 's';
    case DIK_D: return uppercase ? 'D' : 'd';
    case DIK_F: return uppercase ? 'F' : 'f';
    case DIK_G: return uppercase ? 'G' : 'g';
    case DIK_H: return uppercase ? 'H' : 'h';
    case DIK_J: return uppercase ? 'J' : 'j';
    case DIK_K: return uppercase ? 'K' : 'k';
    case DIK_L: return uppercase ? 'L' : 'l';
    case DIK_SEMICOLON: return uppercase ? ':' : ';';
    case DIK_APOSTROPHE: return uppercase ? '"' : '\'';;
    case DIK_GRAVE: return uppercase ? '~' : '`';
    case DIK_BACKSLASH: return uppercase ? '|' : '\\';
    case DIK_Z: return uppercase ? 'Z' : 'z';
    case DIK_X: return uppercase ? 'X' : 'x';
    case DIK_C: return uppercase ? 'C' : 'c';
    case DIK_V: return uppercase ? 'V' : 'v';
    case DIK_B: return uppercase ? 'B' : 'b';
    case DIK_N: return uppercase ? 'N' : 'n';
    case DIK_M: return uppercase ? 'M' : 'm';
    case DIK_COMMA: return uppercase ? '<' : ',';
    case DIK_PERIOD: return uppercase ? '>' : '.';
    case DIK_SLASH: return uppercase ? '?' : '/';
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Input::CharToKey(char c)
{
    switch(c)
    {
    case ' ': return DIK_SPACE;
    case '!': return DIK_1;
    case '1': return DIK_1;
    case '@': return DIK_2;
    case '2': return DIK_2;
    case '#': return DIK_3;
    case '3': return DIK_3;
    case '$': return DIK_4;
    case '4': return DIK_4;
    case '%': return DIK_5;
    case '5': return DIK_5;
    case '^': return DIK_6;
    case '6': return DIK_6;
    case '&': return DIK_7;
    case '7': return DIK_7;
    case '*': return DIK_8;
    case '8': return DIK_8;
    case '(': return DIK_9;
    case '9': return DIK_9;
    case ')': return DIK_0;
    case '0': return DIK_0;
    case '_': return DIK_MINUS;
    case '-': return DIK_MINUS;
    case '+': return DIK_EQUALS;
    case '=': return DIK_EQUALS;
    case 'Q': return DIK_Q;
    case 'q': return DIK_Q;
    case 'W': return DIK_W;
    case 'w': return DIK_W;
    case 'E': return DIK_E;
    case 'e': return DIK_E;
    case 'R': return DIK_R;
    case 'r': return DIK_R;
    case 'T': return DIK_T;
    case 't': return DIK_T;
    case 'Y': return DIK_Y;
    case 'y': return DIK_Y;
    case 'U': return DIK_U;
    case 'u': return DIK_U;
    case 'I': return DIK_I;
    case 'i': return DIK_I;
    case 'O': return DIK_O;
    case 'o': return DIK_O;
    case 'P': return DIK_P;
    case 'p': return DIK_P;
    case '{': return DIK_LBRACKET;
    case '[': return DIK_LBRACKET;
    case '}': return DIK_RBRACKET;
    case ']': return DIK_RBRACKET;
    case 'A': return DIK_A;
    case 'a': return DIK_A;
    case 'S': return DIK_S;
    case 's': return DIK_S;
    case 'D': return DIK_D;
    case 'd': return DIK_D;
    case 'F': return DIK_F;
    case 'f': return DIK_F;
    case 'G': return DIK_G;
    case 'g': return DIK_G;
    case 'H': return DIK_H;
    case 'h': return DIK_H;
    case 'J': return DIK_J;
    case 'j': return DIK_J;
    case 'K': return DIK_K;
    case 'k': return DIK_K;
    case 'L': return DIK_L;
    case 'l': return DIK_L;
    case ':': return DIK_SEMICOLON;
    case ';': return DIK_SEMICOLON;
    case '"': return DIK_APOSTROPHE;
    case '\'': return DIK_APOSTROPHE;
    case '~': return DIK_GRAVE;
    case '`': return DIK_GRAVE;
    case '|': return DIK_BACKSLASH;
    case '\\': return DIK_BACKSLASH;
    case 'Z': return DIK_Z;
    case 'z': return DIK_Z;
    case 'X': return DIK_X;
    case 'x': return DIK_X;
    case 'C': return DIK_C;
    case 'c': return DIK_C;
    case 'V': return DIK_V;
    case 'v': return DIK_V;
    case 'B': return DIK_B;
    case 'b': return DIK_B;
    case 'N': return DIK_N;
    case 'n': return DIK_N;
    case 'M': return DIK_M;
    case 'm': return DIK_M;
    case '<': return DIK_COMMA;
    case ',': return DIK_COMMA;
    case '>': return DIK_PERIOD;
    case '.': return DIK_PERIOD;
    case '?': return DIK_SLASH;
    case '/': return DIK_SLASH;
    }
    return -1;
}
