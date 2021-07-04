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
#include "log.h"
#include <fstream>
#include <iomanip>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
namespace
{
    std::ofstream log_file_;

    std::string Timestamp()
    {
        SYSTEMTIME sys_time;
        GetLocalTime(&sys_time);

        std::ostringstream oss;
        oss << "[" << std::setfill('0') << std::setw(2) << sys_time.wHour
            << ":" << std::setfill('0') << std::setw(2) << sys_time.wMinute
            << ":" << std::setfill('0') << std::setw(2) << sys_time.wSecond
            << "." << std::setfill('0') << std::setw(3) << sys_time.wMilliseconds
            << "] ";

        return oss.str();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Log::Open(const std::string& filename)
{
    log_file_.open(filename.c_str(), std::ios_base::trunc | std::ios_base::out);
}

///////////////////////////////////////////////////////////////////////////////
void Log::Close()
{
    log_file_.close();
}

///////////////////////////////////////////////////////////////////////////////
void Log::Write(const std::string& text)
{
    log_file_ << Timestamp() << text;
}
