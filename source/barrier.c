/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file barrier.c
 *   Implementation of internal thread barrier functionality.
 *****************************************************************************/

#include "align.h"
#include "barrier.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "barrier.h" for documentation.

void* spindleAllocateLocalThreadBarriers(uint32_t taskCount)
{
    if (NULL == spindleLocalBarrierBase)
    {
        // Create a single memory region of size 2x the number of tasks, so that each task gets a counter and a flag.
        spindleLocalBarrierBase = (SSpindleBarrierData*)aligned_malloc(sizeof(SSpindleBarrierData) * taskCount * 2, sizeof(SSpindleBarrierData));
    }
    
    return spindleLocalBarrierBase;
}

// --------

void spindleFreeLocalThreadBarriers(void)
{
    if (NULL != spindleLocalBarrierBase)
    {
        aligned_free((void*)spindleLocalBarrierBase);
        spindleLocalBarrierBase = NULL;
    }
}
