GDTR DW 0 ; For limit storage
     DD 0 ; For base storage

global gdt_install:function
gdt_install:
   ;cli 
   mov   eax, [esp + 4]
   mov   [GDTR + 2], eax
   mov   ax, [esp + 8]
   mov   [GDTR], ax
   lgdt  [GDTR]
   jmp 0x08:reload_CS
reload_CS:
   ; Reload data segment registers:
   mov   ax, 0x10 ; 0x10 points at the new data selector
   mov   ds, ax
   mov   es, ax
   mov   fs, ax
   mov   gs, ax
   mov   ss, ax
   ret
