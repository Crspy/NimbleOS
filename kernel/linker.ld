ENTRY(_start)

KERNEL_START = 0x00100000;
KERNEL_OFFSET = 0xC0000000;

SECTIONS
{
	. = KERNEL_START + KERNEL_OFFSET;
	KERNEL_BEGIN = .;
	KERNEL_BEGIN_PHYS = . - KERNEL_OFFSET;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text : AT(ADDR(.text) - KERNEL_OFFSET)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_OFFSET)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_OFFSET)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss : AT(ADDR(.bss) - KERNEL_OFFSET)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	KERNEL_END = .;
	KERNEL_END_PHYS = . - KERNEL_OFFSET;
	KERNEL_SIZE = KERNEL_END - KERNEL_BEGIN;

	/* The compiler may produce other sections, put them in the proper place in
	   in this file, if you'd like to include them in the final kernel. */
}
