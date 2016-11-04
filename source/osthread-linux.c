/*****************************************************************************
 * libspindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file osthread-linux.c
 *   Implementation of functions for creating and managing OS threads.
 *   This file contains Linux-specific functions.
 *****************************************************************************/

#include "../spindle.h"
#include "init.h"
#include "osthread.h"
#include "types.h"

#include <hwloc.h>
#include <pthread.h>
#include <stdint.h>


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Internal thread start function for Linux.
/// Affinitizes the thread to the required logical core, initializes thread information, and invokes the user-supplied function.
/// @param [arg] Thread specificaion, cast as a typeless pointer.
/// @return `NULL` upon completion of the user-supplied code.
static void* spindleInternalThreadStartFuncLinux(void* arg)
{
    SSpindleThreadInfo* threadSpec = (SSpindleThreadInfo*)arg;

    // Affinitize the thread as required by the thread specification.
    spindleAffinitizeCurrentOSThread(threadSpec->topology, threadSpec->affinityObject);

    // Initialize thread identification information.
    spindleSetThreadID(threadSpec->localThreadID, threadSpec->globalThreadID, threadSpec->taskID);
    spindleSetThreadCounts(threadSpec->localThreadCount, threadSpec->globalThreadCount, threadSpec->taskCount);
    spindleInitializeLocalVariable();

    // Wait for all threads, then call the real thread starting function.
    spindleBarrierGlobal();
    threadSpec->func(threadSpec->arg);
    
    return NULL;
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "osthread.h" for documentation.

hwloc_thread_t spindleCreateOSThread(SSpindleThreadInfo* threadSpec)
{
    pthread_t threadHandle;
    
    if (0 != pthread_create(&threadHandle, NULL, &spindleInternalThreadStartFuncLinux, (void*)threadSpec))
        return (hwloc_thread_t)NULL;
    
    return threadHandle;
}

// --------

hwloc_thread_t spindleIdentifyCurrentOSThread(void)
{
    return (hwloc_thread_t)pthread_self();
}

// --------

uint32_t spindleJoinThreads(SSpindleThreadInfo* threadSpec, uint32_t threadCount)
{
    for (uint32_t i = 0; i < threadCount; ++i)
    {
        if (0 != pthread_join((pthread_t)threadSpec[i].threadHandle, NULL))
            return __LINE__;
    }
    
    return 0;
}
