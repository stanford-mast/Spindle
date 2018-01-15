;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Spindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; spindle.inc
;   Declaration of external API functions and helpers.
;   Top-level assembly-language header file for this library.
;   To be included externally. This version is for MASM on Windows.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFNDEF __SPINDLE_INC
__SPINDLE_INC EQU 1


; --------- MACROS ------------------------------------------------------------
; These assembly-language macros perform the same functions as the similarly-named external API functions.
; For the sake of performance, it is recommended that they be used over the API functions.

; Retrieves the local thread ID and places it in the specified 32-bit register.
spindleAsmHelperGetLocalThreadID MACRO edest
    vpextrd edest, xmm15, 0
ENDM

; Retrieves the global thread ID and places it in the specified 32-bit register.
spindleAsmHelperGetGlobalThreadID MACRO edest
    vpextrd edest, xmm15, 1
ENDM

; Retrieves the task ID and places it in the specified 32-bit register.
spindleAsmHelperGetTaskID MACRO edest
    vpextrd edest, xmm15, 2
ENDM

; Retrieves the number of threads in the current thread's task and places it in the specified 32-bit register.
spindleAsmHelperGetLocalThreadCount MACRO edest
    vpextrd edest, xmm15, 3
ENDM

; Retrieves the total number of threads and places it in the specified 32-bit register.
spindleAsmHelperGetGlobalThreadCount MACRO edest
    vextractf128 xmm0, ymm15, 1
    vpextrd edest, xmm0, 0
ENDM

; Retrieves the total number of tasks and places it in the specified 32-bit register.
spindleAsmHelperGetTaskCount MACRO edest
    vextractf128 xmm0, ymm15, 1
    vpextrd edest, xmm0, 1
ENDM

; Sets the per-thread 64-bit variable from the specified 64-bit source register.
spindleAsmHelperSetLocalVariable MACRO rsrc
    vextractf128 xmm0, ymm15, 1
    vpinsrq xmm0, xmm0, rsrc, 1
    vinsertf128 ymm15, ymm15, xmm0, 1
ENDM

; Retrieves the per-thread 64-bit variable and places it in the specified 64-bit register.
spindleAsmHelperGetLocalVariable MACRO rdest
    vextractf128 xmm0, ymm15, 1
    vpextrq rdest, xmm0, 1
ENDM


; --------- FUNCTIONS ---------------------------------------------------------
; See "spindle.h" for documentation.

EXTRN spindleThreadsSpawn:PROC

EXTRN spindleIsInParallelRegion:PROC

EXTRN spindleGetLocalThreadID:PROC

EXTRN spindleGetGlobalThreadID:PROC

EXTRN spindleGetTaskID:PROC

EXTRN spindleGetLocalThreadCount:PROC

EXTRN spindleGetGlobalThreadCount:PROC

EXTRN spindleGetTaskCount:PROC

EXTRN spindleSetLocalVariable:PROC

EXTRN spindleGetLocalVariable:PROC

EXTRN spindleBarrierLocal:PROC

EXTRN spindleBarrierGlobal:PROC

EXTRN spindleTimedBarrierLocal:PROC

EXTRN spindleTimedBarrierGlobal:PROC


ENDIF ; __SPINDLE_INC
