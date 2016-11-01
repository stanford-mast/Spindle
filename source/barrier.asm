;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; libspindle
;      Multi-platform topology-aware thread control library.
;      Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; barrier.asm
;      Implementation of thread barrier functionality.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE helpers.inc
INCLUDE registers.inc


_TEXT                                       SEGMENT


; --------- FUNCTIONS ---------------------------------------------------------
; See "spindle.h" for documentation.

spindleBarrierLocal                         PROC PUBLIC
    ret
spindleBarrierLocal                         ENDP

; ---------

spindleBarrierGlobal                        PROC PUBLIC
    ret
spindleBarrierGlobal                        ENDP

; ---------

spindleTimedBarrierLocal                    PROC PUBLIC
    ; Capture the initial timestamp.
    lfence
    rdtsc
    shl                     rdx,                    32
    or                      rax,                    rdx
    mov                     r8,                     rax
    
    ; Perform the barrier.
    call                    spindleBarrierLocal
    
    ; Capture the final timestamp and calculate the time taken.
    lfence
    rdtsc
    shl                     rdx,                    32
    or                      rax,                    rdx
    sub                     rax,                    r8
    
    mov                     r_retval,               rax
    ret
spindleTimedBarrierLocal                    ENDP

; ---------

spindleTimedBarrierGlobal                   PROC PUBLIC
    ; Capture the initial timestamp.
    lfence
    rdtsc
    shl                     rdx,                    32
    or                      rax,                    rdx
    mov                     r8,                     rax
    
    ; Perform the barrier.
    call                    spindleBarrierGlobal
    
    ; Capture the final timestamp and calculate the time taken.
    lfence
    rdtsc
    shl                     rdx,                    32
    or                      rax,                    rdx
    sub                     rax,                    r8
    
    mov                     r_retval,               rax
    ret
spindleTimedBarrierGlobal                   ENDP


_TEXT                                       ENDS


END
