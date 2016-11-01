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

spindleGetLocalThreadId                     PROC PUBLIC
    spindleAsmHelperGetLocalThreadId                e_retval
    ret
spindleGetLocalThreadId                     ENDP

; ---------

spindleGetGlobalThreadId                    PROC PUBLIC
    spindleAsmHelperGetGlobalThreadId               e_retval
    ret
spindleGetGlobalThreadId                    ENDP

; ---------

spindleGetThreadGroupId                     PROC PUBLIC
    spindleAsmHelperGetThreadGroupId                e_retval
    ret
spindleGetThreadGroupId                     ENDP

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
