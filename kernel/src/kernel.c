#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/multiboot.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>


void kernel_main(multiboot_info_t *mbi, uint32_t magic)
{
	terminal_init();
	init_gdt();
	init_idt();


	
	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf("Invalid magic number: 0x%x\n", (unsigned)magic);
		return;
	}


	dump_multiboot_info(mbi);

	asm("sti"); // enable interrupts

	// trigger divide by 0 exception
	volatile int x = 0;
	volatile int y = 100/x;
}

