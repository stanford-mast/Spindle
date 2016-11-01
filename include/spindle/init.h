/*****************************************************************************
 * libspindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file init.h
 *   Declaration of internal functions for initializing thread information.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Initializes the calling thread with its local ID, global ID, and thread group ID.
/// Intended to be called internally before passing control to user-supplied code.
/// @param [in] localThreadID Current thread's local ID.
/// @param [in] globalThreadID Current thread's global ID.
/// @param [in] threadGroupID Current thread's group ID.
void spindleSetThreadID(uint32_t localThreadID, uint32_t globalThreadID, uint32_t threadGroupID);

/// Initializes the calling thread with information about the number of other threads.
/// Intended to be called internally before passing control to user-supplied code.
/// @param [in] localThreadCount Number of threads in the current thread's group.
/// @param [in] globalThreadCount Number of threads globally.
/// @param [in] threadGroupCount Number of thread groups globally.
void spindleSetThreadCounts(uint32_t localThreadCount, uint32_t globalThreadCount, uint32_t threadGroupCount);

/// Initializes the calling thread's per-thread local variable to 0.
/// Intended to be called internally before passing control to user-supplied code.
void spindleInitializeLocalVariable(void);
