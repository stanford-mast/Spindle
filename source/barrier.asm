;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Spindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; barrier.asm
;   Implementation of internal thread barrier functionality.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE helpers.inc
INCLUDE registers.inc


DATA                                        SEGMENT ALIGN(64)


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

PUBLIC spindleGlobalBarrierFlag
spindleGlobalBarrierFlag                    DQ          0000000000000000h
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
    ; Each local barrier counter/flag combination is 128 bytes in size, or two cache lines.
    ; Once the address is determined, place the number of threads in the local group into the counter and initialize the flag to 0.
    shl                     r_param1,               7
    add                     r_param1,               QWORD PTR [spindleLocalBarrierBase]
    mov                     DWORD PTR [r_param1+0],                         e_param2
    mov                     DWORD PTR [r_param1+64],                        0
    ret
spindleInitializeLocalThreadBarrier         ENDP

; ---------

spindleInitializeGlobalThreadBarrier        PROC PUBLIC
    ; Place the total number of threads into the counter and initialize the flag to 0.
    mov                     DWORD PTR [spindleGlobalBarrierCounter],        e_param1
    mov                     DWORD PTR [spindleGlobalBarrierFlag],           0
    ret
spindleInitializeGlobalThreadBarrier        ENDP


_TEXT                                       ENDS


END
