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
#include "manager.h"
#include "ihandler.h"

#include "../util/log.h"
#include <iomanip>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#pragma warning(disable : 4311)         // '<function-style-cast>' : pointer truncation from 'Timer::IHandler *' to 'unsigned int'

using namespace Timer;

///////////////////////////////////////////////////////////////////////////////
void Manager::Add(IHandler* handler, unsigned int id, unsigned long timeout)
{
    LOG("Adding timer [" << id << "], timeout [" << timeout << "]");
    AddRemoveTimeout info;
    info.handler_   = handler;
    info.id_        = id;
    info.timeout_   = timeout;
    add_timeouts_.push_back(info);
}

///////////////////////////////////////////////////////////////////////////////
void Manager::Remove(IHandler* handler, int id)
{
    LOG("Removing timer [" << id << "]");
    AddRemoveTimeout info;
    info.handler_   = handler;
    info.id_        = id;
    info.timeout_   = -1;
    remove_timeouts_.push_back(info);
}

///////////////////////////////////////////////////////////////////////////////
void Manager::Remove(IHandler* handler)
{
    AddRemoveTimeout info;
    info.handler_   = handler;
    info.id_        = -1;
    info.timeout_   = -1;
    remove_timeouts_.push_back(info);
}

///////////////////////////////////////////////////////////////////////////////
void Manager::ProcessExpiredTimeouts()
{
    // Add any pending timeouts
    AddRemoveTimeoutList::iterator ar_itor;
    for(ar_itor = add_timeouts_.begin(); ar_itor != add_timeouts_.end(); ++ar_itor)
    {
        Add_i(ar_itor->handler_, ar_itor->id_, ar_itor->timeout_);
    }
    add_timeouts_.clear();

    // Remove any pending timeouts
    for(ar_itor = remove_timeouts_.begin(); ar_itor != remove_timeouts_.end(); ++ar_itor)
    {
        if(ar_itor->id_ != -1)
        {
            Remove_i(ar_itor->handler_, ar_itor->id_);
        }
        else
        {
            Remove_i(ar_itor->handler_);
        }
    }
    remove_timeouts_.clear();

    // Call the handlers for any timeouts that have reached or gone past their
    // expiry times.
    unsigned long now = timeGetTime();
    Timeouts::iterator itor_map;
    for(itor_map = timeouts_.begin(); itor_map != timeouts_.end(); ++itor_map)
    {
        TimeoutInfoList::iterator itor_list;
        for(itor_list = itor_map->second.begin(); itor_list != itor_map->second.end(); ++itor_list)
        {
            if(now >= itor_list->expiry_time_)
            {
                itor_map->first->OnTimeout(itor_list->id_);
                itor_list->dead_ = true;
            }
        }
    }

    // Now remove the dead entries
    itor_map = timeouts_.begin();
    while(itor_map != timeouts_.end())
    {
        TimeoutInfoList::iterator prev;
        TimeoutInfoList::iterator current = itor_map->second.begin();
        while(current != itor_map->second.end())
        {
            prev = current;
            ++current;
            if(prev->dead_)
            {
                itor_map->second.erase(prev);
            }
        }

        if(itor_map->second.empty())
        {
            timeouts_.erase(itor_map);
            itor_map = timeouts_.begin();       // We have to start from the beginning again :-/
        }
        else
        {
            ++itor_map;
        }
    }
}







///////////////////////////////////////////////////////////////////////////////
void Manager::Add_i(IHandler* handler, unsigned int id, unsigned long timeout)
{
    TimeoutInfo new_info;
    new_info.id_            = id;
    new_info.expiry_time_   = timeGetTime() + timeout;

    // Does this handler have at least one other timeout?
    Timeouts::iterator itor = timeouts_.find(handler);
    if(itor == timeouts_.end())
    {
        // No, create a new list.
        TimeoutInfoList timeout_info_list;
        timeout_info_list.push_back(new_info);
        timeouts_[handler] = timeout_info_list;
    }
    else
    {
        // Yes, add a new node to its existing list.
        itor->second.push_back(new_info);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Manager::Remove_i(IHandler* handler, int id)
{
    Timeouts::iterator itor_map = timeouts_.find(handler);
    if(itor_map != timeouts_.end())
    {
        TimeoutInfoList::iterator itor_list;
        for(itor_list = itor_map->second.begin(); itor_list != itor_map->second.end(); ++itor_list)
        {
            if(itor_list->id_ == id)
            {
                itor_map->second.erase(itor_list);
                break;
            }
        }

        if(itor_map->second.empty())
        {
            timeouts_.erase(itor_map);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Manager::Remove_i(IHandler* handler)
{
    Timeouts::iterator itor = timeouts_.find(handler);
    if(itor != timeouts_.end())
    {
        timeouts_.erase(handler);
    }
}
