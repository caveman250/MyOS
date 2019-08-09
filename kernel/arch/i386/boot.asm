; Multiboot constants
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
 
; Declare the multiboot header
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
;Setup a small stack
section .bss
align 16	;16 bit aligned
stack_bottom:
resb 16384 ; 16 KiB, this will grow.
stack_top:
 
;_start is the entry point defined in the linker script. 
section .text
global _start:function (_start.end - _start) ;_start is a global function
_start:
	;point the esp register to the top of the stack
	mov esp, stack_top
 
	extern _init
	call _init

	;call kernel_main in kernel.c. this will only work if the stack is still 16 bit aligned, 
	;since nothing has been pushed to the stack we are ok.
	extern kernel_main
	call kernel_main
 
	;clear interupts and halt
	cli
	hlt
.end:
