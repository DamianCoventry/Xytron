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
#include "xytron.h"

///////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, INT cmd_show)
{
    try
    {
        Xytron xytron(instance);
        xytron.Run();
    }
    catch(std::exception& e)
    {
        MessageBox(GetDesktopWindow(), e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR);
    }

    return 0;
}
