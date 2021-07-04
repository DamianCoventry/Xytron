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
#include "guidutil.h"
#include <objbase.h>

///////////////////////////////////////////////////////////////////////////////
bool Util::GuidToString(const GUID& guid, std::string* str)
{
    OLECHAR wide_str[256];
    memset(wide_str, 0, sizeof(OLECHAR) * 256);
    if(StringFromGUID2(guid, wide_str, 256) <= 0)
    {
        return false;
    }

    char char_str[256];
    memset(char_str, 0, sizeof(char) * 256);
    if(WideCharToMultiByte(CP_ACP, 0, wide_str, 256, char_str, 256, NULL, NULL) <= 0)
    {
        return false;
    }

    if(str)
    {
        str->assign(char_str);
        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
bool Util::StringToGuid(const std::string& str, GUID* guid)
{
    OLECHAR wide_str[256];
    memset(wide_str, 0, sizeof(OLECHAR) * 256);

    if(MultiByteToWideChar(CP_ACP, 0, str.c_str(), int(str.size()), wide_str, 256) <= 0)
    {
        return false;
    }

    return SUCCEEDED(CLSIDFromString(wide_str, guid));
}
