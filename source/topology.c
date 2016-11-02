/*****************************************************************************
 * libspindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file topology.h
 *   Implementation of functions for interacting with the hwloc topology.
 *****************************************************************************/

#include <hwloc.h>
#include <stdlib.h>


// -------- LOCALS --------------------------------------------------------- //

/// Holds the hwloc system topology object.
/// Exposes it as a global variable that can be lazily initialized and freed whenever required.
static hwloc_topology_t spindleSystemTopology = NULL;


// -------- FUNCTIONS ------------------------------------------------------ //
// See "topology.h" for documentation.

hwloc_topology_t spindleGetSystemTopologyObject(void)
{
    if (NULL == spindleSystemTopology)
    {
        // Create and load the hardware topology of the current system.
        if (0 != hwloc_topology_init(&spindleSystemTopology))
            spindleSystemTopology = NULL;
        
        if (NULL == spindleSystemTopology || 0 != hwloc_topology_load(spindleSystemTopology))
            spindleSystemTopology = NULL;
    }
    
    return spindleSystemTopology;
}

// --------

void spindleDestroySystemTopologyObject(void)
{
    if (NULL != spindleSystemTopology)
    {
        hwloc_topology_destroy(spindleSystemTopology);
        spindleSystemTopology = NULL;
    }
}
