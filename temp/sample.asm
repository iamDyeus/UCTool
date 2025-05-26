; Target Code (x86 Assembly)
; Generated on: Mon May 26 20:32:28 2025
; ============================================================

section .data
str1: db ""a: %d, b: %d\n"", 0

section .text
global _start

    ; Load value
    MOV r1, 5
    ; Store value
    MOV [a], r1
    ; Load value
    MOV r2, 10
    ; Store value
    MOV [b], r2
    ; Store value
    MOV [temp], a
func_main:
    ; Load value
    MOV r1, 5
    ; Store value
    MOV [a], r1
    ; Load value
    MOV r2, 10
    ; Store value
    MOV [b], r2
    ; Store value
    MOV [temp], a
    ; Load value
    MOV r3, b
    ; Store value
    MOV [a], r3
    ; Load value
    MOV r4, temp
    ; Store value
    MOV [b], r4
    ; Load value
    MOV t1, ""a: %d, b: %d\n""
    ; Call function
    CALL printf
    ; Load value
    MOV t2, 0
    ; Return
    MOV RAX, t2
    RET
    ; End function
    RET

_start:
    ; Program entry point
    CALL func_main
    MOV RAX, 60
    XOR RDI, RDI
    SYSCALL
; ============================================================
