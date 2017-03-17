;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Spindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; spindle.asm
;   Implementation of most external API functions, except for barriers.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE helpers.inc
INCLUDE registers.inc


_TEXT                                       SEGMENT


; --------- FUNCTIONS ---------------------------------------------------------
; See "spindle.h" for documentation.

spindleGetLocalThreadID                     PROC PUBLIC
    spindleAsmHelperGetLocalThreadID                e_retval
    ret
spindleGetLocalThreadID                     ENDP

; ---------

spindleGetGlobalThreadID                    PROC PUBLIC
    spindleAsmHelperGetGlobalThreadID               e_retval
    ret
spindleGetGlobalThreadID                    ENDP

; ---------

spindleGetTaskID                            PROC PUBLIC
    spindleAsmHelperGetTaskID                       e_retval
    ret
spindleGetTaskID                            ENDP

; ---------

spindleGetLocalThreadCount                  PROC PUBLIC
    spindleAsmHelperGetLocalThreadCount             e_retval
    ret
spindleGetLocalThreadCount                  ENDP

; ---------

spindleGetGlobalThreadCount                 PROC PUBLIC
    spindleAsmHelperGetGlobalThreadCount            e_retval
    ret
spindleGetGlobalThreadCount                 ENDP

; ---------

spindleGetTaskCount                         PROC PUBLIC
    spindleAsmHelperGetTaskCount                    e_retval
    ret
spindleGetTaskCount                         ENDP

; ---------

spindleSetLocalVariable                     PROC PUBLIC
    spindleAsmHelperSetLocalVariable                r_param1
    ret
spindleSetLocalVariable                     ENDP

; ---------

spindleGetLocalVariable                     PROC PUBLIC
    spindleAsmHelperGetLocalVariable                r_retval
    ret
spindleGetLocalVariable                     ENDP


_TEXT                                       ENDS


END
