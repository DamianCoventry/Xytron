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
#include "mouse.h"
#include "../util/throw.h"

using namespace Inp;

///////////////////////////////////////////////////////////////////////////////
Mouse::Mouse()
: dinput_(NULL)
{
    device_ = NULL;
    memset(&state_, 0,sizeof(DIMOUSESTATE));
    memset(&old_state_, 0,sizeof(DIMOUSESTATE));
}

///////////////////////////////////////////////////////////////////////////////
Mouse::~Mouse()
{
}

///////////////////////////////////////////////////////////////////////////////
void Mouse::Initialise(IDirectInput8* dinput, HWND window)
{
    LOG("Initialising the DirectInput mouse device");
    dinput_ = dinput;

    HRESULT hr = dinput_->CreateDevice(GUID_SysMouse, &device_, NULL);
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of GUID_SysMouse");
    }

    hr = device_->SetDataFormat(&c_dfDIMouse);
    if(FAILED(hr))
    {
        THROW_COM("Could not set the data format for the Mouse device");
    }

    hr = device_->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of GUID_SysMouse");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Mouse::Shutdown()
{
    if(device_)
    {
        LOG("Shutting down the DirectInput mouse device");
        device_->Release();
        device_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Mouse::Activate()
{
    if(device_)
    {
        HRESULT result = device_->Acquire();
        if(FAILED(result))
        {
            LOG("Failed to acquire the DirectInput mouse device");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Mouse::Deactivate()
{
    if(device_)
    {
        device_->Unacquire();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Mouse::FrameBegin()
{
    if(device_)
    {
        HRESULT hr = device_->GetDeviceState(sizeof(DIMOUSESTATE), &state_);
        if(FAILED(hr))
        {
            device_->Acquire();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Mouse::FrameEnd()
{
    memcpy(&old_state_, &state_, sizeof(DIMOUSESTATE));
}

///////////////////////////////////////////////////////////////////////////////
bool Mouse::IsButtonPressed(int button)
{
    return (state_.rgbButtons[button] != 0) && (old_state_.rgbButtons[button] == 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Mouse::IsButtonReleased(int button)
{
    return (state_.rgbButtons[button] == 0) && (old_state_.rgbButtons[button] != 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Mouse::IsButtonHeld(int button)
{
    return (state_.rgbButtons[button] != 0);
}

///////////////////////////////////////////////////////////////////////////////
void Mouse::GetMovement(int* x_delta, int* y_delta)
{
    if(x_delta)
    {
        (*x_delta) = state_.lX;
    }
    if(y_delta)
    {
        (*y_delta) = state_.lY;
    }
}
