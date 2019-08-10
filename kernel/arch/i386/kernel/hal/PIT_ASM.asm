global handle_irq:function
handle_irq:
    pushad
    extern handle_irq_main
    call handle_irq_main
    popad
    iretd