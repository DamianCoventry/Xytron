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
#include "joystick.h"

#include "../util/guidutil.h"
#include "../util/throw.h"

using namespace Inp;

///////////////////////////////////////////////////////////////////////////////
namespace
{
    BOOL PASCAL JoystickCallback(const DIDEVICEOBJECTINSTANCE* object, void* param)
    {
        LOG("Found the joystick axis [" << object->tszName << "]");

        DIPROPRANGE property;
        memset(&property, 0,sizeof(DIPROPRANGE));

        property.diph.dwSize        = sizeof(DIPROPRANGE); 
        property.diph.dwHeaderSize  = sizeof(DIPROPHEADER); 
        property.diph.dwHow         = DIPH_BYID; 
        property.diph.dwObj         = object->dwType; 
        property.lMin               = -Joystick::RANGE; 
        property.lMax               =  Joystick::RANGE; 

        IDirectInputDevice8* device = reinterpret_cast<IDirectInputDevice8*>(param);
        HRESULT hr = device->SetProperty(DIPROP_RANGE, &property.diph);

        return FAILED(hr) ? DIENUM_STOP : DIENUM_CONTINUE;
    }
}


///////////////////////////////////////////////////////////////////////////////
Joystick::Joystick()
: dinput_(NULL)
{
    device_ = NULL;
    memset(&guid_, 0, sizeof(GUID));
    memset(&state_, 0,sizeof(DIJOYSTATE));
    memset(&old_state_, 0,sizeof(DIJOYSTATE));
}

///////////////////////////////////////////////////////////////////////////////
Joystick::~Joystick()
{
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::Initialise(IDirectInput8* dinput, HWND window, const GUID& guid)
{
    std::string guid_str;
    Util::GuidToString(guid, &guid_str);
    LOG("Initialising the DirectInput joystick device [" << guid_str << "]");

    dinput_ = dinput;
    memcpy(&guid_, &(guid), sizeof(GUID));

    HRESULT hr = dinput_->CreateDevice(guid_, &device_, NULL);
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of a joystick")
    }

    hr = device_->SetDataFormat(&c_dfDIJoystick);
    if(FAILED(hr))
    {
        THROW_COM("Could not set the data format for the Joystick device")
    }

    hr = device_->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of GUID_SysJoystick")
    }

    LOG("Enumerating the joystick's axies");
    hr = device_->EnumObjects(JoystickCallback, reinterpret_cast<void*>(device_), DIDFT_AXIS);
    if(FAILED(hr))
    {
        THROW_COM("Could not enumerate the axies of the Joystick")
    }
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::Shutdown()
{
    if(device_)
    {
        LOG("Shutting down the DirectInput joystick device");
        device_->Release();
        device_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::Activate()
{
    if(device_)
    {
        HRESULT result = device_->Acquire();
        if(FAILED(result))
        {
            LOG("Failed to acquire the DirectInput joystick device");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::Deactivate()
{
    if(device_)
    {
        device_->Unacquire();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::FrameBegin()
{
    if(device_)
    {
        HRESULT hr = device_->Poll();
        if(FAILED(hr))
        {
            device_->Acquire();
            return;
        }

        hr = device_->GetDeviceState(sizeof(DIJOYSTATE), &state_);
        if(FAILED(hr))
        {
            device_->Acquire();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::FrameEnd()
{
    memcpy(&old_state_, &state_, sizeof(DIJOYSTATE));
}

///////////////////////////////////////////////////////////////////////////////
bool Joystick::IsButtonPressed(int button)
{
    return (state_.rgbButtons[button] != 0) && (old_state_.rgbButtons[button] == 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Joystick::IsButtonReleased(int button)
{
    return (state_.rgbButtons[button] == 0) && (old_state_.rgbButtons[button] != 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Joystick::IsButtonHeld(int button)
{
    return (state_.rgbButtons[button] != 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Joystick::IsPovButtonPressed(Direction dir, int pov)
{
    int pos;
    switch(dir)
    {
    case DIR_UP:    pos =     0; break;
    case DIR_RIGHT: pos =  9000; break;
    case DIR_DOWN:  pos = 18000; break;
    case DIR_LEFT:  pos = 27000; break;
    }
    return (state_.rgdwPOV[pov] == pos) && (old_state_.rgdwPOV[pov] != pos);
}

///////////////////////////////////////////////////////////////////////////////
bool Joystick::IsPovButtonReleased(Direction dir, int pov)
{
    int pos;
    switch(dir)
    {
    case DIR_UP:    pos =     0; break;
    case DIR_RIGHT: pos =  9000; break;
    case DIR_DOWN:  pos = 18000; break;
    case DIR_LEFT:  pos = 27000; break;
    }
    return (state_.rgdwPOV[pov] != pos) && (old_state_.rgdwPOV[pov] == pos);
}

///////////////////////////////////////////////////////////////////////////////
bool Joystick::IsPovButtonHeld(Direction dir, int pov)
{
    int pos;
    switch(dir)
    {
    case DIR_UP:    pos =     0; break;
    case DIR_RIGHT: pos =  9000; break;
    case DIR_DOWN:  pos = 18000; break;
    case DIR_LEFT:  pos = 27000; break;
    }
    return (state_.rgdwPOV[pov] == pos);
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::GetMovement(int* x_delta, int* y_delta, int* z_delta)
{
    if(x_delta)
    {
        (*x_delta) = state_.lX;
    }
    if(y_delta)
    {
        (*y_delta) = state_.lY;
    }
    if(z_delta)
    {
        (*z_delta) = state_.lZ;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Joystick::GetRotation(int* x_delta, int* y_delta, int* z_delta)
{
    if(x_delta)
    {
        (*x_delta) = state_.lRx;
    }
    if(y_delta)
    {
        (*y_delta) = state_.lRy;
    }
    if(z_delta)
    {
        (*z_delta) = state_.lRz;
    }
}

///////////////////////////////////////////////////////////////////////////////
int Joystick::GetSliderPos(int slider)
{
    if(slider == 0 || slider == 1)
    {
        return state_.rglSlider[slider];
    }
    return 0;
}
