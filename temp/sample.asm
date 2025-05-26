; Target Code (x86 Assembly)
; Generated on: Mon May 26 20:37:44 2025
; ============================================================

section .data
str1: db ""Hello, World! GREET\n"", 0
str2: db ""Hi there from INT MAIN!\n"", 0

section .text
global _start

func_greet:
    ; Load value
    MOV r1, ""Hello, World! GREET\n""
    ; Call function
    CALL printf
    ; End function
    RET
func_main:
    ; Load value
    MOV r1, ""Hi there from INT MAIN!\n""
    ; Call function
    CALL printf
    ; Call function
    CALL greet
    ; Load value
    MOV r2, 0
    ; Return
    MOV RAX, r2
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
