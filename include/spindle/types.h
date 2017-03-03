/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file types.h
 *   Declaration of internal types.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include "../spindle.h"

#include <hwloc.h>
#include <stdint.h>


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Internal data structure, used to provide each spawned thread with control and identification information.
/// One such data structure exists per thread created during the spawning process.
/// Each instance uniquely identifies and supplies sufficient information for each thread.
/// These data structures are created as threads are assigned to logical cores and read by each thread's internal starting function.
typedef struct SSpindleThreadInfo
{
    TSpindleFunc func;                                                      ///< Starting function to call.
    void* arg;                                                              ///< Argument to pass to the starting function.

    hwloc_topology_t topology;                                              ///< System topology object from `hwloc`.
    hwloc_obj_t affinityObject;                                             ///< Object from `hwloc` that identifies the PU to which the present thread should be affinitized.

    uint32_t localThreadID;                                                 ///< Local thread ID.
    uint32_t globalThreadID;                                                ///< Global thread ID.
    uint32_t taskID;                                                        ///< Task ID.
    uint32_t localThreadCount;                                              ///< Number of threads in the current task.
    uint32_t globalThreadCount;                                             ///< Total number of threads spawned.
    uint32_t taskCount;                                                     ///< Total number of tasks created.

    hwloc_thread_t threadHandle;                                            ///< Thread handle, used to identify and wait for threads once they are created.
} SSpindleThreadInfo;
