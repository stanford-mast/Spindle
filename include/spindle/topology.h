/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file topology.h
 *   Declaration of functions for interacting with the `hwloc` topology.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include <hwloc.h>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Retrieves and returns the system topology object handle.
/// This function will lazily instantiate the system topology object if it is not yet instantiated.
/// @return System topology object handle from `hwloc`, or `NULL` in the event of an instantiation failure.
hwloc_topology_t spindleGetSystemTopologyObject(void);

/// Retrieves and returns the system topology object that corresponds to the specified NUMA node, by `hwloc` index.
/// This is useful because some single-node systems lack NUMA node objects entirely.
/// @param [in] numaNodeIndex `hwloc` index of the NUMA node of interest.
/// @return NUMA node object handle from `hwloc`, or `NULL` in the event of a failure (object does not exist, instantiation failure, etc.).
hwloc_obj_t spindleGetNUMANodeObjectAtIndex(uint32_t numaNodeIndex);

/// Destroys and frees all system resources held to maintain the `hwloc` system topology.
/// This function is idempotent and can be invoked anytime outside of a code region parallelized by this library.
void spindleDestroySystemTopologyObject(void);
