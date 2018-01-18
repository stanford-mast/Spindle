/*****************************************************************************
* Spindle
*   Multi-platform topology-aware thread control library.
*   Distributes a set of synchronized tasks over cores in the system.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016-2017
*************************************************************************//**
* @file datashare.h
*   Interface to internal data sharing functionality.
*   Not intended for external use.
*****************************************************************************/

#pragma once

#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Allocates space for all data sharing buffers.
/// Intended to be called during the spawning process.
/// @param [in] taskCount Number of tasks.
/// @return Pointer to the start of the memory region on success, or `NULL` on failure.
void* spindleAllocateDataShareBuffers(uint32_t taskCount);

/// Frees all previously-allocated space for data sharing buffers.
/// Intended to be called after all spawned threads have terminated.
void spindleFreeDataShareBuffers(void);
