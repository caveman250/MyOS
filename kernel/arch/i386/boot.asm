;Multiboot constants
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
 
;Declare the multiboot header
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
section .bss
align 4096 ;4KB aligned
boot_page_directory:
resb 4096
boot_page_table1:
resb 4096
boot_page_table2:
resb 4096

;Setup a small stack
align 16	;16 bit aligned
stack_bottom:
resb 16384 ; 16 KiB, this will grow.
stack_top:
 
;_start is the entry point defined in the linker script. 
section .text
global _start:function ;_start is a global function
_start:
	pusha
	;map the kernel
	mov		ecx, 1024							; 1024 page entires = 4mb
	mov 	eax, 0
	mov 	ebx, 0x0
	or ebx, 0x003
.maploop1:
	mov	dword [dword boot_page_table1 - 0xC0000000 + (eax * 4)], ebx
	inc eax
	add ebx, 4096
	loop .maploop1

	;map kernel to 0xc0000000 (3GB)
	mov dword [dword boot_page_directory - 0xC0000000 + (768 * 4)], boot_page_table1 - 0xC0000000 + 0x003
	popa

	pusha
	;map the first 4 mb
	mov		ecx, 1024							; 1024 page entires = 4mb
	mov 	eax, 0
	mov 	ebx, 0x0 | 0x003
.maploop2:
	mov	dword [dword boot_page_table2 - 0xC0000000 + eax * 4], ebx
	inc eax
	add ebx, 4096
	loop .maploop2

	;map first table to 0x0 (indentity mapped)
	mov dword [dword boot_page_directory - 0xC0000000], boot_page_table2 - 0xC0000000 + 0x003
	popa

	;Set cr3 to the address of the boot_page_directory.
	mov ecx, boot_page_directory - 0xC0000000
	mov cr3, ecx
	
	;Enable paging
	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	;Jump to higher half with an absolute jump. 
	lea ecx, [dword higher_half]
	jmp ecx

higher_half:
	;kernel is now in the higher half
	mov ecx, cr3
	mov cr3, ecx

	;point the esp register to the top of the stack
	mov esp, stack_top

	;push GRUB multiboot info onto the stack
	push dword ebx
	push dword eax
 
	;initialise global variables
	extern _init
	call _init

	;call kernel_main in kernel.cpp. this will only work if the stack is still 16 bit aligned
	extern kernel_main
	call kernel_main
 
	cli
	hlt
