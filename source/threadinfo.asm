;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; libspindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; threadinfo.asm
;   Implementation of external API functions that deal with thread info.
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

spindleGetThreadGroupID                     PROC PUBLIC
    spindleAsmHelperGetThreadGroupID                e_retval
    ret
spindleGetThreadGroupID                     ENDP

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

spindleGetGroupCount                        PROC PUBLIC
    spindleAsmHelperGetGroupCount                   e_retval
    ret
spindleGetGroupCount                        ENDP

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
