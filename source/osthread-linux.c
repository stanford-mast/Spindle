/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file osthread-linux.c
 *   Implementation of functions for creating and managing OS threads.
 *   This file contains Linux-specific functions.
 *****************************************************************************/

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
    spindleRunThreadSpec((SSpindleThreadInfo*)arg);
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

// --------

uint32_t spindleStartCurrentThread(SSpindleThreadInfo* threadSpec)
{
    spindleInternalThreadStartFuncLinux((void*)threadSpec);
    return 0;
}
