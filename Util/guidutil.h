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
#ifndef INCLUDED_UTIL_GUIDUTIL
#define INCLUDED_UTIL_GUIDUTIL

#include <guiddef.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace Util
{

bool GuidToString(const GUID& guid, std::string* str);
bool StringToGuid(const std::string& str, GUID* guid);

}       // namespace Util

#endif  // INCLUDED_SAGE_UTILITY_GUIDUTIL
