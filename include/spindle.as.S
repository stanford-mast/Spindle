###############################################################################
# Spindle
#   Multi-platform topology-aware thread control library.
#   Distributes a set of synchronized tasks over cores in the system.
###############################################################################
# Authored by Samuel Grossman
# Department of Electrical Engineering, Stanford University
# Copyright (c) 2016-2017
###############################################################################
# spindle.as.S
#   Declaration of external API functions and helpers.
#   Top-level assembly-language header file for this library.
#   To be included externally. This version is for GNU AS on Linux.
###############################################################################

.ifndef __SPINDLE_INC
.equ __SPINDLE_INC, 1


.intel_syntax noprefix


# --------- MACROS ------------------------------------------------------------
# These assembly-language macros perform the same functions as the similarly-named external API functions.
# For the sake of performance, it is recommended that they be used over the API functions.

# Retrieves the local thread ID and places it in the specified 32-bit register.
.macro spindleAsmHelperGetLocalThreadID edest
    vpextrd \edest, xmm15, 0
.endm

# Retrieves the global thread ID and places it in the specified 32-bit register.
.macro spindleAsmHelperGetGlobalThreadID edest
    vpextrd \edest, xmm15, 1
.endm

# Retrieves the task ID and places it in the specified 32-bit register.
.macro spindleAsmHelperGetTaskID edest
    vpextrd \edest, xmm15, 2
.endm

# Retrieves the number of threads in the current thread's task and places it in the specified 32-bit register.
.macro spindleAsmHelperGetLocalThreadCount edest
    vpextrd \edest, xmm15, 3
.endm

# Retrieves the total number of threads and places it in the specified 32-bit register.
.macro spindleAsmHelperGetGlobalThreadCount edest
    vextractf128 xmm0, ymm15, 1
    vpextrd \edest, xmm0, 0
.endm

# Retrieves the total number of tasks and places it in the specified 32-bit register.
.macro spindleAsmHelperGetTaskCount edest
    vextractf128 xmm0, ymm15, 1
    vpextrd \edest, xmm0, 1
.endm

# Sets the per-thread 64-bit variable from the specified 64-bit source register.
.macro spindleAsmHelperSetLocalVariable rsrc
    vextractf128 xmm0, ymm15, 1
    vpinsrq xmm0, xmm0, \rsrc, 1
    vinsertf128 ymm15, ymm15, xmm0, 1
.endm

# Retrieves the per-thread 64-bit variable and places it in the specified 64-bit register.
.macro spindleAsmHelperGetLocalVariable rdest
    vextractf128 xmm0, ymm15, 1
    vpextrq \rdest, xmm0, 1
.endm


# --------- FUNCTIONS ---------------------------------------------------------
# See "spindle.h" for documentation.

.extern spindleThreadsSpawn

.extern spindleGetLocalThreadID

.extern spindleGetGlobalThreadID

.extern spindleGetTaskID

.extern spindleGetLocalThreadCount

.extern spindleGetGlobalThreadCount

.extern spindleGetTaskCount

.extern spindleSetLocalVariable

.extern spindleGetLocalVariable

.extern spindleBarrierLocal

.extern spindleBarrierGlobal

.extern spindleTimedBarrierLocal

.extern spindleTimedBarrierGlobal


.endif # __SPINDLE_INC
