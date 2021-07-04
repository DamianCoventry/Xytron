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
#ifndef INCLUDED_UTIL_THROW
#define INCLUDED_UTIL_THROW

#include "log.h"
#include <stdexcept>
#include <iomanip>

#define CHECK_GL(ostream_text_)         \
{                                       \
    GLenum gl_error = glGetError();     \
    if(gl_error != GL_NO_ERROR)         \
    {                                   \
        THROW_OPENGL(ostream_text_);    \
    }                                   \
}

///////////////////////////////////////////////////////////////////////////////
#define THROW(ostream_text_)                                                        \
{                                                                                   \
    std::ostringstream oss_throw_;                                                  \
    oss_throw_ << "\n"                                                              \
               << "************** EXCEPTION THROWN **************\n"                \
               << "File:    " << __FILE__ << "\n"                                   \
               << "Line:    " << __LINE__ << "\n"                                   \
               << "Type:    General Exception\n"                                    \
               << "Text:    " << ostream_text_ << "\n"                              \
               << "**********************************************\n";               \
    Log::Write(oss_throw_.str());                                                   \
    oss_throw_.str("");                                                             \
    oss_throw_ << ostream_text_;                                                    \
    throw std::runtime_error(oss_throw_.str());                                     \
}

///////////////////////////////////////////////////////////////////////////////
#define THROW_WIN32(ostream_text_)                                                  \
{                                                                                   \
    std::ostringstream oss_throw_;                                                  \
    oss_throw_ << "\n"                                                              \
               << "************** EXCEPTION THROWN **************\n"                \
               << "File:         " << __FILE__ << "\n"                              \
               << "Line:         " << __LINE__ << "\n"                              \
               << "Type:         Win32 Exception\n"                                 \
               << "GetLastError: " << GetLastError() << "\n"                        \
               << "Text:         " << ostream_text_ << "\n"                         \
               << "**********************************************\n";               \
    Log::Write(oss_throw_.str());                                                   \
    oss_throw_.str("");                                                             \
    oss_throw_ << ostream_text_;                                                    \
    throw std::runtime_error(oss_throw_.str());                                     \
}

///////////////////////////////////////////////////////////////////////////////
#define THROW_OPENGL(ostream_text_)                                                 \
{                                                                                   \
    std::ostringstream oss_throw_;                                                  \
    oss_throw_ << "\n"                                                              \
               << "************** EXCEPTION THROWN **************\n"                \
               << "File:       " << __FILE__ << "\n"                                \
               << "Line:       " << __LINE__ << "\n"                                \
               << "Type:       OpenGL Exception\n"                                  \
               << "glGetError: " << (char*)gluErrorString(gl_error) << "\n"         \
               << "Text:       " << ostream_text_ << "\n"                           \
               << "**********************************************\n";               \
    Log::Write(oss_throw_.str());                                                   \
    oss_throw_.str("");                                                             \
    oss_throw_ << ostream_text_;                                                    \
    throw std::runtime_error(oss_throw_.str());                                     \
}

///////////////////////////////////////////////////////////////////////////////
#define THROW_COM(ostream_text_)                                                    \
{                                                                                   \
    std::ostringstream oss_throw_;                                                  \
    oss_throw_ << "\n"                                                              \
               << "************** EXCEPTION THROWN **************\n"                \
               << "File:    " << __FILE__ << "\n"                                   \
               << "Line:    " << __LINE__ << "\n"                                   \
               << "Type:    COM Exception\n"                                        \
               << "HRESULT: 0x" << std::hex << std::setw(8) << std::setfill('0') << hr << "\n"  \
               << "Text:    " << ostream_text_ << "\n"                              \
               << "**********************************************\n";               \
    Log::Write(oss_throw_.str());                                                   \
    oss_throw_.str("");                                                             \
    oss_throw_ << ostream_text_;                                                    \
    throw std::runtime_error(oss_throw_.str());                                     \
}

///////////////////////////////////////////////////////////////////////////////
#define THROW_DSOUND(ostream_text_)                                                 \
{                                                                                   \
    std::ostringstream oss_throw_;                                                  \
    oss_throw_ << "\n"                                                              \
               << "************** EXCEPTION THROWN **************\n"                \
               << "File:    " << __FILE__ << "\n"                                   \
               << "Line:    " << __LINE__ << "\n"                                   \
               << "Type:    DirectSound Exception\n"                                \
               << "HRESULT: 0x" << std::hex << std::setw(8) << std::setfill('0') << hr << "\n"  \
               << "Text:    " << ostream_text_ << "\n"                              \
               << "**********************************************\n";               \
    Log::Write(oss_throw_.str());                                                   \
    oss_throw_.str("");                                                             \
    oss_throw_ << ostream_text_;                                                    \
    throw std::runtime_error(oss_throw_.str());                                     \
}

///////////////////////////////////////////////////////////////////////////////
#define THROW_DINPUT(ostream_text_)                                                 \
{                                                                                   \
    std::ostringstream oss_throw_;                                                  \
    oss_throw_ << "\n"                                                              \
               << "************** EXCEPTION THROWN **************\n"                \
               << "File:    " << __FILE__ << "\n"                                   \
               << "Line:    " << __LINE__ << "\n"                                   \
               << "Type:    DirectInput Exception\n"                                \
               << "HRESULT: 0x" << std::hex << std::setw(8) << std::setfill('0') << hr << "\n"  \
               << "Text:    " << ostream_text_ << "\n"                              \
               << "**********************************************\n";               \
    Log::Write(oss_throw_.str());                                                   \
    oss_throw_.str("");                                                             \
    oss_throw_ << ostream_text_;                                                    \
    throw std::runtime_error(oss_throw_.str());                                     \
}

#endif  // INCLUDED_UTIL_THROW
