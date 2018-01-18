/*****************************************************************************
* Spindle
*   Multi-platform topology-aware thread control library.
*   Distributes a set of synchronized tasks over cores in the system.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016-2017
*************************************************************************//**
* @file datashare.c
*   Implementation of internal data sharing functionality.
*****************************************************************************/

#include "../spindle.h"
#include "align.h"
#include "datashare.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Represents the layout of storage space used to hold data to be shared between threads.
/// A single 64-bit value is accompanied by padding, to align on a two-cache-line (128-byte) boundary.
typedef struct SSpindleDataShareBuffer
{
    uint64_t data;                                                          ///< Shared data value.
    uint8_t padding[128 - sizeof(uint64_t)];                                ///< Unused, cache-line alignment padding.
} SSpindleDataShareBuffer;


// -------- LOCALS --------------------------------------------------------- //

/// Storage area for all data sharing buffers.
/// The last position is to be used for the global data sharing buffer, others are for local sharing within each task.
static SSpindleDataShareBuffer* spindleDataShareBufferBase;


// -------- FUNCTIONS ------------------------------------------------------ //
// See "datashare.h" for documentation.

void* spindleAllocateDataShareBuffers(uint32_t taskCount)
{
    if (NULL == spindleDataShareBufferBase)
    {
        // Create a single memory region with one data sharing buffer per task, plus one for the global data sharing buffer.
        spindleDataShareBufferBase = (SSpindleDataShareBuffer*)aligned_malloc(sizeof(SSpindleDataShareBuffer) * (1 + taskCount), sizeof(SSpindleDataShareBuffer));
    }

    return spindleDataShareBufferBase;
}

// --------

void spindleFreeDataShareBuffers(void)
{
    if (NULL != spindleDataShareBufferBase)
    {
        aligned_free((void*)spindleDataShareBufferBase);
        spindleDataShareBufferBase = NULL;
    }
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "spindle.h" for documentation.

void spindleDataShareSendLocal(uint64_t data)
{
    spindleDataShareBufferBase[spindleGetTaskID()].data = data;
    spindleBarrierLocal();
}

// --------

void spindleDataShareSendGlobal(uint64_t data)
{
    spindleDataShareBufferBase[spindleGetTaskCount()].data = data;
    spindleBarrierGlobal();
}

// --------

uint64_t spindleDataShareReceiveLocal(void)
{
    spindleBarrierLocal();
    return spindleDataShareBufferBase[spindleGetTaskID()].data;
}

// --------

uint64_t spindleDataShareReceiveGlobal(void)
{
    spindleBarrierGlobal();
    return spindleDataShareBufferBase[spindleGetTaskCount()].data;
}
