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
#ifndef INCLUDED_AUD_DEVICELIST
#define INCLUDED_AUD_DEVICELIST

#include <guiddef.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace Aud
{

///////////////////////////////////////////////////////////////////////////////
struct DeviceInfo
{
    GUID guid_;
    std::string name_;
};

typedef std::vector<DeviceInfo> DeviceInfoList;

///////////////////////////////////////////////////////////////////////////////
struct DeviceList
{
    static DeviceInfoList Get();
};

}       // namespace Aud

#endif  // INCLUDED_AUD_DEVICELIST
