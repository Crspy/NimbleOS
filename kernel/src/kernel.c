#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/multiboot.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/cpu.h>

void kernel_main(multiboot_info_t *, uint32_t magic)
{
	terminal_init();
	gdt_init();
	idt_init();
	irq_init();
	
	timer_init();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf("Invalid magic number: 0x%x\n", (unsigned)magic);
		return;
	}

	double start = timer_get_time();
	double dur_s = 2;
	while (1)
	{
		double now = timer_get_time();
		if ((now - start) >= dur_s)
		{
			start = now;
			printf("%d s passed\n", (int)dur_s); // this will get printed every 2 seconds
		}
		HLT();
	}
}
