global keyboard_irq:function
keyboard_irq:
	pushad
    extern keyboard_irq_main
    call keyboard_irq_main
	popad
	iretd