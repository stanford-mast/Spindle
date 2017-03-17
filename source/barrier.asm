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

PUBLIC spindleInternalGlobalBarrierCounter
spindleInternalGlobalBarrierCounter         DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h
                                            DQ          0000000000000000h

PUBLIC spindleInternalGlobalBarrierFlag
spindleInternalGlobalBarrierFlag            DQ          0000000000000000h
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


; --------- MACROS ------------------------------------------------------------

; Implements a thread barrier.
; Invoked by the various routines that expose thread barriers to the library user.
; Register parameters: ecx (number of threads for which to wait), r8 (memory address of barrier counter), r9 (memory address of barrier flag)
; Macro parameters: label to use for the internal loop, label to use for completion
; Internally uses and overwrites eax and edx.
spindleBarrier                              MACRO labelLoop, labelDone
    ; Read in the current value of the thread barrier flag.
    mov                     edx,                    DWORD PTR [r9]

    ; Atomically decrement the thread barrier counter and start waiting if needed.
    mov                     eax,                    0ffffffffh
    lock xadd               DWORD PTR [r8],         eax
    jne                     labelLoop

    ; If all other threads have been here, clean up and signal them to wake up.
    mov                     DWORD PTR [r8],         ecx
    inc                     DWORD PTR [r9]
    jmp                     labelDone

    ; Wait here for the signal.
  labelLoop:
    pause
    cmp                     edx,                    DWORD PTR [r9]
    je                      labelLoop
    
  labelDone:
ENDM


; --------- FUNCTIONS ---------------------------------------------------------
; See "barrier.h" and "spindle.h" for documentation.

spindleBarrierLocal                         PROC PUBLIC
    ; Calculate the memory address within the local barrier memory region for the current thread's barrier counter and flag.
    ; This is based on the thread's task ID.
    spindleAsmHelperGetTaskID                       r8d
    shl                     r8,                     7
    add                     r8,                     QWORD PTR [spindleLocalBarrierBase]
    mov                     r9,                     r8
    add                     r9,                     64
    
    ; Number of threads for which to wait is equal to the number of threads that exist locally in the current task.
    spindleAsmHelperGetLocalThreadCount             ecx
    
    ; Invoke the barrier itself.
    spindleBarrier          spindleBarrierLocal_Loop,                       spindleBarrierLocal_Done

	; All threads in the present task have passed the barrier.
    ret
spindleBarrierLocal                         ENDP

; ---------

spindleBarrierGlobal                        PROC PUBLIC
    ; Obtain the addresses of counter and flag.
    lea                     r8,                     QWORD PTR [spindleGlobalBarrierCounter]
    lea                     r9,                     QWORD PTR [spindleGlobalBarrierFlag]
    
    ; Number of threads for which to wait is equal to the number of threads that exist globally.
    spindleAsmHelperGetGlobalThreadCount            ecx
    
    ; Invoke the barrier itself.
    spindleBarrier          spindleBarrierGlobal_Loop,                      spindleBarrierGlobal_Done

	; All threads globally have passed the barrier.
    ret
spindleBarrierGlobal                        ENDP

; ---------

spindleBarrierInternalGlobal                PROC PUBLIC
    ; Obtain the addresses of counter and flag.
    lea                     r8,                     QWORD PTR [spindleInternalGlobalBarrierCounter]
    lea                     r9,                     QWORD PTR [spindleInternalGlobalBarrierFlag]
    
    ; Number of threads for which to wait is equal to the number of threads that exist globally.
    spindleAsmHelperGetGlobalThreadCount            ecx
    
    ; Invoke the barrier itself.
    spindleBarrier          spindleBarrierInternalGlobal_Loop,              spindleBarrierInternalGlobal_Done
    
	; All threads globally have passed the barrier.
    ret
spindleBarrierInternalGlobal                ENDP

; ---------

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
    
    mov                     DWORD PTR [spindleInternalGlobalBarrierCounter],                        e_param1
    mov                     DWORD PTR [spindleInternalGlobalBarrierFlag],                           0
    ret
spindleInitializeGlobalThreadBarrier        ENDP

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
