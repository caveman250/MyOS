/*Let the bootloader know to start at the symbol "_start" */
ENTRY(_start)
 
SECTIONS
{
	/* Start from 1 MegaByte */
	. = 1M;

	kernelStart = .;

	/* put the multiboot section first, followed by the text section */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	kernelEnd= .;
}
