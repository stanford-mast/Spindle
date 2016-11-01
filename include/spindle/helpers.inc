;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; libspindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; helpers.inc
;   Assembly macros for handling thread information.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFNDEF __SPINDLE_HELPERS_INC
__SPINDLE_HELPERS_INC EQU 1


INCLUDE registers.inc


; --------- MACROS ------------------------------------------------------------

; Retrieves the local thread ID and places it in the specified 32-bit register.
spindleAsmHelperGetLocalThreadId            MACRO edest
    vpextrd                 edest,                  xmm_threadinfo,         0
ENDM

; Retrieves the global thread ID and places it in the specified 32-bit register.
spindleAsmHelperGetGlobalThreadId           MACRO edest
    vpextrd                 edest,                  xmm_threadinfo,         1
ENDM

; Retrieves the thread group ID and places it in the specified 32-bit register.
spindleAsmHelperGetThreadGroupId            MACRO edest
    vpextrd                 edest,                  xmm_threadinfo,         2
ENDM

; Retrieves the number of threads in the current thread's group and places it in the specified 32-bit register.
spindleAsmHelperGetLocalThreadCount         MACRO edest
    vpextrd                 edest,                  xmm_threadinfo,         3
ENDM

; Retrieves the total number of threads and places it in the specified 32-bit register.
spindleAsmHelperGetGlobalThreadCount        MACRO edest
    vextractf128            xmm0,                   ymm_threadinfo,         1
    vpextrd                 edest,                  xmm0,                   0
ENDM

; Retrieves the total number of thread groups and places it in the specified 32-bit register.
spindleAsmHelperGetGroupCount               MACRO edest
    vextractf128            xmm0,                   ymm_threadinfo,         1
    vpextrd                 edest,                  xmm0,                   1
ENDM

; Sets the per-thread 64-bit variable from the specified 64-bit source register.
spindleAsmHelperSetLocalVariable            MACRO rsrc
    vextractf128            xmm0,                   ymm_threadinfo,         1
    vpinsrq                 xmm0,                   xmm0,                   rsrc,                   1
    vinsertf128             ymm_threadinfo,         ymm_threadinfo,         xmm0,                   1
ENDM

; Retrieves the per-thread 64-bit variable and places it in the specified 64-bit register.
spindleAsmHelperGetLocalVariable            MACRO rdest
    vextractf128            xmm0,                   ymm_threadinfo,         1
    vpextrq                 rdest,                  xmm0,                   1
ENDM


ENDIF ; __SPINDLE_HELPERS_INC
