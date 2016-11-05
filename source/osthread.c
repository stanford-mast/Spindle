/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file osthread.c
 *   Implementation of functions for creating and managing OS threads.
 *   This file contains platform-independent functions.
 *****************************************************************************/

#include "osthread.h"
#include "types.h"

#include <hwloc.h>
#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "osthread.h" for documentation.

void spindleAffinitizeCurrentOSThread(hwloc_topology_t topology, hwloc_obj_t affinityObject)
{
    hwloc_set_thread_cpubind(topology, spindleIdentifyCurrentOSThread(), affinityObject->cpuset, HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_STRICT);
}

// --------

uint32_t spindleCreateThreads(SSpindleThreadInfo* threadSpec, uint32_t threadCount)
{
    for (uint32_t i = 0; i < threadCount; ++i)
    {
        threadSpec[i].threadHandle = spindleCreateOSThread(&threadSpec[i]);
        
        if ((hwloc_thread_t)NULL == threadSpec[i].threadHandle)
            return __LINE__;
    }
    
    return spindleJoinThreads(threadSpec, threadCount);
}
