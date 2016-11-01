/*****************************************************************************
 * libspindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file barrier.h
 *   Interface to internal thread barrier functionality.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Initializes the local thread barrier memory regions for the specified thread group.
/// Intended to be called during the thread spawning process but before actual thread creation.
/// @param [in] threadGroupID Target thread group ID.
/// @param [in] localThreadCount Number of threads being spawned in the target thread group.
void spindleInitializeLocalThreadBarrier(uint32_t threadGroupID, uint32_t localThreadCount);

/// Initializes the global thread barrier memory regions.
/// Intended to be called during the thread spawning process but before actual thread creation.
/// @param [in] globalThreadCount Number of threads being spawned globally.
void spindleInitializeGlobalThreadBarrier(uint32_t globalThreadCount);
