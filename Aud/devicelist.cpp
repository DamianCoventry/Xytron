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
#include "devicelist.h"

#include "../util/guidutil.h"
#include "../util/throw.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

using namespace Aud;

///////////////////////////////////////////////////////////////////////////////
namespace
{
    BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext)
    {
        DeviceInfoList* devices = reinterpret_cast<DeviceInfoList*>(lpContext);

        DeviceInfo info;
        if(lpGUID)
        {
            memcpy(&info.guid_, lpGUID, sizeof(GUID));
        }
        else
        {
            memset(&info.guid_, 0, sizeof(GUID));
        }

        info.name_ = lpszDesc;
        devices->push_back(info);

        return TRUE;
    }
}

///////////////////////////////////////////////////////////////////////////////
DeviceInfoList DeviceList::Get()
{
    DeviceInfoList devices;

    LOG("Enumerating audio playback devices");

    DirectSoundEnumerate(DSEnumProc, reinterpret_cast<void*>(&devices));

    if(devices.empty())
    {
        LOG("There are no audio playback devices attached to this computer");
    }
    else
    {
        LOG("Found [" << int(devices.size()) << "] audio playback devices");
        DeviceInfoList::iterator itor;
        for(itor = devices.begin(); itor != devices.end(); ++itor)
        {
            std::string guid_str;
            Util::GuidToString(itor->guid_, &guid_str);
            LOG("Audio playback device [" << guid_str << "][" << itor->name_ << "]");
        }
    }

    return devices;
}

