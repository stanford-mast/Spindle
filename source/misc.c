/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file misc.c
 *   Implementation of miscellaneous external API functions.
 *****************************************************************************/

#include "../spindle.h"
#include "topology.h"

#include <hwloc.h>
#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "spindle.h" for documentation.

uint32_t spindleGetSystemNUMANodeCount(void)
{
    hwloc_topology_t topology = spindleGetSystemTopologyObject();
    uint32_t numNumaNodes = 0;

    if (NULL != topology)
    {
        numNumaNodes = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_NUMANODE);
        
        if (0 == numNumaNodes)
            numNumaNodes = 1;
    }    
    
    return numNumaNodes;
}

// --------

uint32_t spindleGetNUMANodePhysicalCoreCount(uint32_t numaNodeIndex)
{
    hwloc_topology_t topology = spindleGetSystemTopologyObject();
    hwloc_obj_t numaNodeObject = NULL;
    
    if (NULL == topology)
        return 0;
    
    numaNodeObject = spindleGetNUMANodeObjectAtIndex(numaNodeIndex);
    if (NULL == numaNodeObject)
        return 0;
    
    return hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE);
}

// --------

uint32_t spindleGetNUMANodeMaxThreadCount(uint32_t numaNodeIndex)
{
    hwloc_topology_t topology = spindleGetSystemTopologyObject();
    hwloc_obj_t numaNodeObject = NULL;
    
    if (NULL == topology)
        return 0;
    
    numaNodeObject = spindleGetNUMANodeObjectAtIndex(numaNodeIndex);
    if (NULL == numaNodeObject)
        return 0;
    
    return hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_PU);
}
