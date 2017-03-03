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

#include "barrier.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>


// -------- PLATFORM-SPECIFIC MACROS --------------------------------------- //

/// Allocates a memory region of size `size` bytes aligned to an `align`-byte boundary.
/// Implementation is platform-specific.
#ifdef SPINDLE_WINDOWS
#define aligned_malloc(size, align)             _aligned_malloc(size, align)
#else
#define aligned_malloc(size, align)             memalign(align, size)
#endif

/// Frees an aligned memory region.
/// Implementation is platform-specific.
#ifdef SPINDLE_WINDOWS    
#define aligned_free(ptr)                       _aligned_free(ptr)
#else
#define aligned_free(ptr)                       free(ptr)
#endif

 
// -------- FUNCTIONS ------------------------------------------------------ //
// See "barrier.h" for documentation.

void* spindleAllocateLocalThreadBarriers(uint32_t taskCount)
{
    if (NULL == spindleLocalBarrierBase)
    {
        // Create a single memory region of size 2x the number of tasks, so that each task gets a counter and a flag.
        void* localBarrierMemoryRegion = aligned_malloc(sizeof(SSpindleBarrierData) * taskCount * 2, sizeof(SSpindleBarrierData));
        
        if (NULL != localBarrierMemoryRegion)
        {
            spindleLocalBarrierBase = (SSpindleBarrierData*)localBarrierMemoryRegion;
        }
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
