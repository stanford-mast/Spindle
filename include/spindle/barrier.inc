;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; libspindle
;   Multi-platform topology-aware thread control library.
;   Distributes a set of synchronized tasks over cores in the system.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; barrier.inc
;   Interface to internal thread barrier memory regions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFNDEF __SPINDLE_BARRIER_INC
__SPINDLE_BARRIER_INC EQU 1


INCLUDE registers.inc


; --------- GLOBALS -----------------------------------------------------------

; Storage area for the counter of threads that have reached the global barrier, plus cache-line padding.
EXTRN spindleGlobalBarrierCounter:QWORD

; Storage area for the global barrier flag, on which threads spin while waiting for the global barrier, plus cache-line padding.
EXTRN spindleGlobalBarrierFlag:QWORD


ENDIF ; __SPINDLE_BARRIER_INC
