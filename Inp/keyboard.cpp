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
#include "keyboard.h"
#include "../util/throw.h"

using namespace Inp;

///////////////////////////////////////////////////////////////////////////////
Keyboard::Keyboard()
: dinput_(NULL)
{
    device_ = NULL;
    memset(keys_, 0, sizeof(char)*NUM_KEYS);
    memset(old_keys_, 0, sizeof(char)*NUM_KEYS);
}

///////////////////////////////////////////////////////////////////////////////
Keyboard::~Keyboard()
{
}

///////////////////////////////////////////////////////////////////////////////
void Keyboard::Initialise(IDirectInput8* dinput, HWND window)
{
    LOG("Initialising the DirectInput keyboard device");
    dinput_ = dinput;

    HRESULT hr = dinput_->CreateDevice(GUID_SysKeyboard, &device_, NULL);
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of GUID_SysKeyboard");
    }

    hr = device_->SetDataFormat(&c_dfDIKeyboard);
    if(FAILED(hr))
    {
        THROW_COM("Could not set the data format for the keyboard device");
    }

    hr = device_->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if(FAILED(hr))
    {
        THROW_COM("Could not create an instance of GUID_SysKeyboard");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Keyboard::Shutdown()
{
    if(device_)
    {
        LOG("Shutting down the DirectInput keyboard device");
        device_->Release();
        device_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Keyboard::Activate()
{
    if(device_)
    {
        HRESULT result = device_->Acquire();
        if(FAILED(result))
        {
            LOG("Failed to acquire the DirectInput keyboard device");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Keyboard::Deactivate()
{
    if(device_)
    {
        device_->Unacquire();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Keyboard::FrameBegin()
{
    if(device_)
    {
        HRESULT hr = device_->GetDeviceState(sizeof(char)*NUM_KEYS, keys_);
        if(FAILED(hr))
        {
            device_->Acquire();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Keyboard::FrameEnd()
{
    memcpy(old_keys_, keys_, sizeof(char)*NUM_KEYS);
}

///////////////////////////////////////////////////////////////////////////////
bool Keyboard::IsKeyPressed(int key)
{
    return ((keys_[key] & 0x80) != 0) && ((old_keys_[key] & 0x80) == 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Keyboard::IsKeyReleased(int key)
{
    return ((keys_[key] & 0x80) == 0) && ((old_keys_[key] & 0x80) != 0);
}

///////////////////////////////////////////////////////////////////////////////
bool Keyboard::IsKeyHeld(int key)
{
    return ((keys_[key] & 0x80) != 0);
}
