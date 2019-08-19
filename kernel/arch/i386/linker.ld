/*Let the bootloader know to start at the symbol "_start" */
ENTRY(_start)
 
SECTIONS
{
	/* Start from 1 MegaByte */
	. = 0xC0100000;

	kernelStart = .;

	/* put the multiboot section first, followed by the text section */
	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}
 
	/* Read-only data. */
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	kernelEnd= .;
}
