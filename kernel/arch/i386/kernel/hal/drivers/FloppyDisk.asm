global floppy_disk_handle_irq:function
floppy_disk_handle_irq:
	pushad
    extern floppy_disk_handle_irq_main
    call floppy_disk_handle_irq_main
	popad
	iretd