/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file barrier.h
 *   Interface to internal thread barrier functionality.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include <stdint.h>


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Represents the layout of storage space used to hold barrier-related quantities.
/// A single 32-bit value is accompanied by padding, to align on a two-cache-line (128-byte) boundary.
typedef struct SSpindleBarrierData
{
    uint32_t value;                                                         ///< Data value.
    uint8_t padding[128 - sizeof(uint32_t)];                                ///< Unused, cache-line alignment padding.
} SSpindleBarrierData;


// -------- GLOBALS -------------------------------------------------------- //

/// Storage area for the counter of threads that have reached the global barrier, plus cache-line padding.
extern SSpindleBarrierData spindleGlobalBarrierCounter;

/// Storage area for the global barrier flag, on which threads spin while waiting for the global barrier, plus cache-line padding.
extern SSpindleBarrierData spindleGlobalBarrierFlag;

/// Base address for all local barrier counters and flags.
extern SSpindleBarrierData* spindleLocalBarrierBase;


// -------- FUNCTIONS ------------------------------------------------------ //

/// Allocates space for all local thread barriers.
/// Intended to be called during the spawning process.
/// @param [in] taskCount Number of tasks.
/// @return Pointer to the start of the memory region on success, or `NULL` on failure.
void* spindleAllocateLocalThreadBarriers(uint32_t taskCount);

/// Provides a barrier that no thread can pass until all threads have reached this point in the execution.
/// For internal use only. This is the same as the external version, except it uses a different area of memory to help catch end-user bugs.
/// If a user specifies tasks with different numbers of global barriers, Spindle needs a separate internal barrier to help avoid allowing the program to proceed past thread spawning.
void spindleBarrierInternalGlobal(void);

/// Frees all previously-allocated space for local thread barriers.
/// Intended to be called after all spawned threads have terminated.
void spindleFreeLocalThreadBarriers(void);

/// Initializes the local thread barrier memory regions for the specified thread group.
/// Intended to be called during the thread spawning process but before actual thread creation.
/// @param [in] taskID Target thread group ID.
/// @param [in] localThreadCount Number of threads being spawned in the target thread group.
void spindleInitializeLocalThreadBarrier(uint32_t taskID, uint32_t localThreadCount);

/// Initializes the global thread barrier memory regions.
/// Intended to be called during the thread spawning process but before actual thread creation.
/// @param [in] globalThreadCount Number of threads being spawned globally.
void spindleInitializeGlobalThreadBarrier(uint32_t globalThreadCount);
