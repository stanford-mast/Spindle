;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; libspindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; barrier.asm
;   Implementation of internal thread barrier functionality.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE helpers.inc
INCLUDE registers.inc


DATA                                        SEGMENT ALIGN(128)


; --------- GLOBALS -----------------------------------------------------------
; See "barrier.h" for documentation.

PUBLIC spindleGlobalBarrierCounter
spindleGlobalBarrierCounter                 DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h

PUBLIC spindleGlobalBarrierFlag
spindleGlobalBarrierFlag                    DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h

PUBLIC spindleLocalBarrierBase
spindleLocalBarrierBase                     DQ          0000000000000000h


DATA                                        ENDS


_TEXT                                       SEGMENT


; --------- FUNCTIONS ---------------------------------------------------------
; See "barrier.h" for documentation.

spindleInitializeLocalThreadBarrier         PROC PUBLIC
    ret
spindleInitializeLocalThreadBarrier         ENDP

; ---------

spindleInitializeGlobalThreadBarrier        PROC PUBLIC
    mov                     DWORD PTR [spindleGlobalBarrierCounter],        e_param1
    mov                     DWORD PTR [spindleGlobalBarrierFlag],           0
    ret
spindleInitializeGlobalThreadBarrier        ENDP


_TEXT                                       ENDS


END
