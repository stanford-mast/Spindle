;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Spindle
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
; See "barrier.h" for documentation.

EXTRN spindleGlobalBarrierCounter:QWORD

EXTRN spindleGlobalBarrierFlag:QWORD

EXTRN spindleLocalBarrierBase:QWORD


ENDIF ; __SPINDLE_BARRIER_INC
