;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Spindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; init.asm
;   Implementation of internal functions for initializing thread information.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE helpers.inc
INCLUDE registers.inc


_TEXT                                       SEGMENT


; --------- FUNCTIONS ---------------------------------------------------------
; See "init.h" for documentation.

spindleSetThreadID                          PROC PUBLIC
    vpinsrd                 xmm_threadinfo,         xmm_threadinfo,         e_param1,               0           ; Local thread ID
    vpinsrd                 xmm_threadinfo,         xmm_threadinfo,         e_param2,               1           ; Global thread ID
    vpinsrd                 xmm_threadinfo,         xmm_threadinfo,         e_param3,               2           ; Task ID
    ret
spindleSetThreadID                          ENDP

; ---------

spindleSetThreadCounts                      PROC PUBLIC
    vpinsrd                 xmm_threadinfo,         xmm_threadinfo,         e_param1,               3           ; Number of threads in the current task
    vpinsrd                 xmm0,                   xmm0,                   e_param2,               0           ; Number of threads globally
    vpinsrd                 xmm0,                   xmm0,                   e_param3,               1           ; Number of tasks globally
    vinsertf128             ymm_threadinfo,         ymm_threadinfo,         xmm0,                   1
    ret
spindleSetThreadCounts                      ENDP

; ---------

spindleInitializeLocalVariable              PROC PUBLIC
    xor                     rax,                    rax
    spindleAsmHelperSetLocalVariable                rax
    ret
spindleInitializeLocalVariable              ENDP


_TEXT                                       ENDS


END
