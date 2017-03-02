/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file osthread.h
 *   Declaration of functions for creating and managing OS threads.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include "types.h"

#include <hwloc.h>
#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Affinitizes the calling OS thread to the specified hwloc affinity object within the specified hwloc topology.
/// @param [in] topology `hwloc` system topology.
/// @param [in] affinityObject `hwloc` object within the topology object that represents the PU to which to affinitize.
void spindleAffinitizeCurrentOSThread(hwloc_topology_t topology, hwloc_obj_t affinityObject);

/// Creates a single OS thread per the thread specification.
/// This is a platform-specific operation.
/// @param [in] threadSpec Thread specification.
/// @return OS-specific handle that identifies the newly-created thread.
hwloc_thread_t spindleCreateOSThread(SSpindleThreadInfo* threadSpec);

/// Creates the threads specified by the thread specifications and thread count.
/// Returns once all created threads have terminated or an error occurs.
/// @param [in, out] threadSpec Array of thread assignment specifications. The threadHandle members are filled with thread identification information during this function.
/// @param [in] threadCount Number of threads to create.
/// @return 0 once all threads terminate successfully, or nonzero in the event of an error.
uint32_t spindleCreateThreads(SSpindleThreadInfo* threadSpec, uint32_t threadCount);

/// Retrieves the OS-specific handle that identifes the calling thread.
/// This is a platform-specific operation.
/// @return OS-specific handle identifying the calling thread.
hwloc_thread_t spindleIdentifyCurrentOSThread(void);

/// Joins the specified threads, returning only once they have all terminated or an error occurs.
/// This is a platform-specific operation.
/// @param [in] threadSpec Array of thread assignment specifications. Only the threadHandle member is used, and it must be filled for all elements.
/// @param [in] threadCount Number of threads in the threadSpec array.
/// @return 0 once all threads terminate successfully, or nonzero in the event of an error.
uint32_t spindleJoinThreads(SSpindleThreadInfo* threadSpec, uint32_t threadCount);

/// Applies the thread specification to the current thread and executes its user-specified function.
/// This is a platform-specific operation.
/// @param [in] threadSpec Thread specification.
/// @return 0 once the user-supplied function returns.
uint32_t spindleStartCurrentThread(SSpindleThreadInfo* threadSpec);
