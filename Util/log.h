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
#ifndef INCLUDED_UTIL_LOG
#define INCLUDED_UTIL_LOG

#include <string>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
#define LOG(ostream_text_)              \
{                                       \
    std::ostringstream oss_log_;        \
    oss_log_ << ostream_text_ << "\n";  \
    Log::Write(oss_log_.str());         \
}

///////////////////////////////////////////////////////////////////////////////
namespace Log
{

    void Open(const std::string& filename);
    void Close();
    void Write(const std::string& text);

}       // namespace Log

#endif  // INCLUDED_UTIL_LOG
