.def WRITE  0x0002 ; Interrupt code for writing to a device
.def SCREEN 0x0001 ; Device ID for the screen

.section .data
    .label hellostr:
        .ascii "Hello, World!"
    .def hellostr_len 13

.section .text
.global _start
_start:
SEW R0, WRITE
S32 R1, hellostr
SEW R3, hellostr_len
SEW R4, SCREEN
XOR R5, R5
XOR R6, R6
INT
XOR R0, R0
INT

