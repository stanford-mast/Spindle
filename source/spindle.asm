;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; libspindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; spindle.asm
;   Implementation of external API functions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE barrier.inc
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

; ---------

spindleBarrierLocal                         PROC PUBLIC
    ret
spindleBarrierLocal                         ENDP

; ---------

spindleBarrierGlobal                        PROC PUBLIC
    ; Read in the current value of the thread barrier flag.
    mov                     edx,                    DWORD PTR [spindleGlobalBarrierFlag]

    ; Atomically decrement the thread barrier counter and start waiting if needed.
    mov                     eax,                    0ffffffffh
    lock xadd               DWORD PTR [spindleGlobalBarrierCounter],        eax
    jne                     spindleBarrierGlobal_Loop

    ; If all other threads have been here, clean up and signal them to wake up.
    spindleAsmHelperGetGlobalThreadCount            ecx
    mov                     DWORD PTR [spindleGlobalBarrierCounter],        ecx
    inc                     DWORD PTR [spindleGlobalBarrierFlag]
    jmp                     spindleBarrierGlobal_Done

    ; Wait here for the signal.
  spindleBarrierGlobal_Loop:
    pause
    cmp                     edx,                    DWORD PTR [spindleGlobalBarrierFlag]
    je                      spindleBarrierGlobal_Loop
    
  spindleBarrierGlobal_Done:
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
