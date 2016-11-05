;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Spindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; registers.inc
;   Name mappings for registers used throughout this library.
;   Abstracts away platform-specific calling conventions.
;   Otherwise improves readability of assembly code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFNDEF __SPINDLE_REGISTERS_INC
__SPINDLE_REGISTERS_INC EQU 1


; --------- PLATFORM-SPECIFIC FUNCTION CALLING CONVENTIONS --------------------
; Windows and Linux use different registers for integer parameters and return values.
; These definitions abstract this away.

IFDEF SPINDLE_WINDOWS
; 64-bit parameter and return value registers
r_param1                                    TEXTEQU     <rcx>
r_param2                                    TEXTEQU     <rdx>
r_param3                                    TEXTEQU     <r8>
r_param4                                    TEXTEQU     <r9>
r_retval                                    TEXTEQU     <rax>

; 32-bit parameter and return value registers
e_param1                                    TEXTEQU     <ecx>
e_param2                                    TEXTEQU     <edx>
e_param3                                    TEXTEQU     <r8d>
e_param4                                    TEXTEQU     <r9d>
e_retval                                    TEXTEQU     <eax>
ENDIF

IFDEF SPINDLE_LINUX
; 64-bit parameter and return value registers
r_param1                                    TEXTEQU     <rdi>
r_param2                                    TEXTEQU     <rsi>
r_param3                                    TEXTEQU     <rdx>
r_param4                                    TEXTEQU     <rcx>
r_retval                                    TEXTEQU     <rax>

; 32-bit parameter and return value registers
e_param1                                    TEXTEQU     <edi>
e_param2                                    TEXTEQU     <esi>
e_param3                                    TEXTEQU     <edx>
e_param4                                    TEXTEQU     <ecx>
e_retval                                    TEXTEQU     <eax>
ENDIF


; --------- PURPOSE-NAMED REGISTERS -------------------------------------------
; These definitions are used to identify the purpose of specific reserved registers.
; The intention is to improve code readability.

; Packed register for holding all threading-related information (IDs, per-thread variable, etc.).
; This register is to be considered globally-reserved during all code regions parallelized by this library.
xmm_threadinfo                              TEXTEQU     <xmm15>
ymm_threadinfo                              TEXTEQU     <ymm15>


ENDIF ;__SPINDLE_REGISTERS_INC
