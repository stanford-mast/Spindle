/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file version.c
 *   Implementation of library version API functions.
 *****************************************************************************/

#include "../spindle.h"

#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "spindle.h" for documentation.

uint32_t spindleGetLibraryVersion(void)
{
    return SPINDLE_LIBRARY_VERSION;
}
