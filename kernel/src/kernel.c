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

void kernel_main(multiboot_info_t *mbi, uint32_t magic)
{
	terminal_init();
	gdt_init();
	idt_init();
	irq_init();
	
	timer_init(1000); // 1000HZ -> timer_tick = 1ms

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf("Invalid magic number: 0x%x\n", (unsigned)magic);
		return;
	}

	
	uint32_t start = timer_get_tick();
	uint32_t dur = 1000; 
	while (1)
	{
		uint32_t ntick = timer_get_tick();
		if ((ntick - start) >= dur)
		{
			start = ntick;
			printf("%d ticks passed\n", dur); // this will get printed every 1000 ms
		}
		HLT();
	}
}
