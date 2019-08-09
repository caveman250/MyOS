global idt_install:function
idt_install:
    mov eax, [esp + 4]
    lidt [eax]
    ret
