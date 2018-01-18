/*****************************************************************************
* Spindle
*   Multi-platform topology-aware thread control library.
*   Distributes a set of synchronized tasks over cores in the system.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016-2017
*************************************************************************//**
* @file align.h
*   Platform-specific memory alignment macros.
*   Not intended for external use.
*****************************************************************************/

#pragma once


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
