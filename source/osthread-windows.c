/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file osthread-windows.c
 *   Implementation of functions for creating and managing OS threads.
 *   This file contains Windows-specific functions.
 *****************************************************************************/

#include "osthread.h"
#include "types.h"

#include <hwloc.h>
#include <malloc.h>
#include <stdint.h>
#include <windows.h>


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Internal thread start function for Windows.
/// Affinitizes the thread to the required logical core, initializes thread information, and invokes the user-supplied function.
/// @param [arg] Thread specificaion, cast as a typeless pointer.
/// @return 0 upon completion of the user-supplied code.
static DWORD WINAPI spindleInternalThreadStartFuncWindows(LPVOID arg)
{
    spindleRunThreadSpec((SSpindleThreadInfo*)arg);
    return 0;
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "osthread.h" for documentation.

hwloc_thread_t spindleCreateOSThread(SSpindleThreadInfo* threadSpec)
{
    return CreateThread(NULL, 0, &spindleInternalThreadStartFuncWindows, (LPVOID)threadSpec, 0, NULL);
}

// --------

hwloc_thread_t spindleIdentifyCurrentOSThread(void)
{
    return (hwloc_thread_t)GetCurrentThread();
}

// --------

uint32_t spindleJoinThreads(SSpindleThreadInfo* threadSpec, uint32_t threadCount)
{
    DWORD waitResult = 0;
    HANDLE* threadHandles = (HANDLE*)malloc(sizeof(HANDLE) * threadCount);
    if (NULL == threadHandles)
        return __LINE__;
    
    for (uint32_t i = 0; i < threadCount; ++i)
        threadHandles[i] = threadSpec[i].threadHandle;
    
    waitResult = WaitForMultipleObjects((DWORD)threadCount, threadHandles, TRUE, INFINITE);
    
    free((void*)threadHandles);
    return (uint32_t)waitResult;
}

// --------

uint32_t spindleStartCurrentThread(SSpindleThreadInfo* threadSpec)
{
    spindleInternalThreadStartFuncWindows((LPVOID)threadSpec);
    return 0;
}
