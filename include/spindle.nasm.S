;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Spindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; spindle.nasm.S
;   Declaration of external API functions and helpers.
;   Top-level assembly-language header file for this library.
;   To be included externally. This version is for NASM on Linux.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%ifndef __SPINDLE_INC
%define __SPINDLE_INC 1


; --------- MACROS ------------------------------------------------------------
; These assembly-language macros perform the same functions as the similarly-named external API functions.
; For the sake of performance, it is recommended that they be used over the API functions.

; Retrieves the local thread ID and places it in the specified 32-bit register.
%macro spindleAsmHelperGetLocalThreadID 1
    vpextrd %1, xmm15, 0
%endmacro

; Retrieves the global thread ID and places it in the specified 32-bit register.
%macro spindleAsmHelperGetGlobalThreadID 1
    vpextrd %1, xmm15, 1
%endmacro

; Retrieves the task ID and places it in the specified 32-bit register.
%macro spindleAsmHelperGetTaskID 1
    vpextrd %1, xmm15, 2
%endmacro

; Retrieves the number of threads in the current thread's task and places it in the specified 32-bit register.
%macro spindleAsmHelperGetLocalThreadCount 1
    vpextrd %1, xmm15, 3
%endmacro

; Retrieves the total number of threads and places it in the specified 32-bit register.
%macro spindleAsmHelperGetGlobalThreadCount 1
    vextractf128 xmm0, ymm15, 1
    vpextrd %1, xmm0, 0
%endmacro

; Retrieves the total number of tasks and places it in the specified 32-bit register.
%macro spindleAsmHelperGetTaskCount 1
    vextractf128 xmm0, ymm15, 1
    vpextrd %1, xmm0, 1
%endmacro

; Sets the per-thread 64-bit variable from the specified 64-bit source register.
%macro spindleAsmHelperSetLocalVariable 1
    vextractf128 xmm0, ymm15, 1
    vpinsrq xmm0, xmm0, %1, 1
    vinsertf128 ymm15, ymm15, xmm0, 1
%endmacro

; Retrieves the per-thread 64-bit variable and places it in the specified 64-bit register.
%macro spindleAsmHelperGetLocalVariable 1
    vextractf128 xmm0, ymm15, 1
    vpextrq %1, xmm0, 1
%endmacro


; --------- FUNCTIONS ---------------------------------------------------------
; See "spindle.h" for documentation.

extern spindleThreadsSpawn

extern spindleGetLocalThreadID

extern spindleGetGlobalThreadID

extern spindleGetTaskID

extern spindleGetLocalThreadCount

extern spindleGetGlobalThreadCount

extern spindleGetTaskCount

extern spindleSetLocalVariable

extern spindleGetLocalVariable

extern spindleBarrierLocal

extern spindleBarrierGlobal

extern spindleTimedBarrierLocal

extern spindleTimedBarrierGlobal


%endif ; __SPINDLE_INC
