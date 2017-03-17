/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file osthread.c
 *   Implementation of functions for creating and managing OS threads.
 *   This file contains platform-independent functions.
 *****************************************************************************/

#include "barrier.h"
#include "init.h"
#include "osthread.h"
#include "types.h"

#include <hwloc.h>
#include <stdbool.h>
#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "osthread.h" for documentation.

void spindleAffinitizeCurrentOSThread(hwloc_topology_t topology, hwloc_obj_t affinityObject)
{
    hwloc_set_thread_cpubind(topology, spindleIdentifyCurrentOSThread(), affinityObject->cpuset, HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_STRICT);
}

// --------

uint32_t spindleCreateThreads(SSpindleThreadInfo* threadSpec, uint32_t threadCount, bool useCurrentThread)
{
    if (useCurrentThread)
    {
        for (uint32_t i = 1; i < threadCount; ++i)
        {
            threadSpec[i].threadHandle = spindleCreateOSThread(&threadSpec[i]);

            if ((hwloc_thread_t)NULL == threadSpec[i].threadHandle)
                return __LINE__;
        }

        threadSpec[0].threadHandle = spindleIdentifyCurrentOSThread();
        return spindleStartCurrentThread(&threadSpec[0]);
    }
    else
    {
        for (uint32_t i = 0; i < threadCount; ++i)
        {
            threadSpec[i].threadHandle = spindleCreateOSThread(&threadSpec[i]);

            if ((hwloc_thread_t)NULL == threadSpec[i].threadHandle)
                return __LINE__;
        }

        return spindleJoinThreads(threadSpec, threadCount);
    }
}

// --------

void spindleRunThreadSpec(SSpindleThreadInfo* threadSpec)
{
    // Affinitize the thread as required by the thread specification.
    spindleAffinitizeCurrentOSThread(threadSpec->topology, threadSpec->affinityObject);

    // Initialize thread identification information.
    spindleSetThreadID(threadSpec->localThreadID, threadSpec->globalThreadID, threadSpec->taskID);
    spindleSetThreadCounts(threadSpec->localThreadCount, threadSpec->globalThreadCount, threadSpec->taskCount);
    spindleInitializeLocalVariable();

    // Wait for all threads, then call the real thread starting function.
    spindleBarrierInternalGlobal();
    threadSpec->func(threadSpec->arg);
    spindleBarrierInternalGlobal();
}
