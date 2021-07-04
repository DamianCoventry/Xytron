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
#include "displaymode.h"
#include "../util/throw.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
bool DisplayMode::Set()
{
    DEVMODE dev_mode;
    memset(&dev_mode, 0, sizeof(DEVMODE));
    dev_mode.dmSize             = sizeof(DEVMODE);
    dev_mode.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
    dev_mode.dmPelsWidth        = width_;
    dev_mode.dmPelsHeight       = height_;
    dev_mode.dmBitsPerPel       = bpp_;
    dev_mode.dmDisplayFrequency = hz_;

    return ChangeDisplaySettings(&dev_mode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
}

///////////////////////////////////////////////////////////////////////////////
Gfx::DisplayModeList DisplayModes::Modes()
{
    DisplayModeList modes;

    DEVMODE dev_mode;
    memset(&dev_mode, 0, sizeof(DEVMODE));
    dev_mode.dmSize     = sizeof(DEVMODE);

    DWORD i = 0;
    while(EnumDisplaySettings(NULL, i, &dev_mode))
    {
        modes.push_back(DisplayMode(dev_mode.dmPelsWidth,
                                    dev_mode.dmPelsHeight,
                                    dev_mode.dmBitsPerPel,
                                    dev_mode.dmDisplayFrequency));
        i++;
    }

    return modes;
}

///////////////////////////////////////////////////////////////////////////////
bool DisplayModes::SetModeFromRegistry()
{
    return ChangeDisplaySettings(NULL, 0) == DISP_CHANGE_SUCCESSFUL;
}
