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
#ifndef INCLUDED_TIMERMANAGER
#define INCLUDED_TIMERMANAGER

#include <map>
#include <list>

///////////////////////////////////////////////////////////////////////////////
namespace Timer
{

struct IHandler;

///////////////////////////////////////////////////////////////////////////////
class Manager
{
public:
    void Add(IHandler* handler, unsigned int id, unsigned long timeout);
    void Remove(IHandler* handler, int id);
    void Remove(IHandler* handler);
    void ProcessExpiredTimeouts();

private:
    void Add_i(IHandler* handler, unsigned int id, unsigned long timeout);
    void Remove_i(IHandler* handler, int id);
    void Remove_i(IHandler* handler);

private:
    struct TimeoutInfo
    {
        TimeoutInfo() : dead_(false) {}
        unsigned int id_;
        unsigned long expiry_time_;
        bool dead_;
    };
    typedef std::list<TimeoutInfo> TimeoutInfoList;
    typedef std::map<IHandler*, TimeoutInfoList> Timeouts;
    Timeouts timeouts_;

    struct AddRemoveTimeout
    {
        IHandler* handler_;
        unsigned int id_;
        unsigned long timeout_;
    };
    typedef std::list<AddRemoveTimeout> AddRemoveTimeoutList;
    AddRemoveTimeoutList add_timeouts_;
    AddRemoveTimeoutList remove_timeouts_;
};

}       // namespace Timer

#endif  // INCLUDED_TIMERMANAGER
