#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/cpu.h>


void kernel_main(multiboot_info_t *, uint32_t magic)
{
	term_init();
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

	printf("Welcome to ");
	term_setcolor(COLOR_WHITE, COLOR_BLACK);
	printf("NimbleOS");
	term_setcolor(COLOR_YELLOW, COLOR_BLACK);
	printf(" v1.0 !\n");

	uint32_t time_start = timer_get_time();
	while (1) {
		uint32_t time_now = timer_get_time();
		if (time_now != time_start) {
			time_start = time_now;
			printf("time: %d\n", time_now); // will print every 1s
			term_change_bg_color(time_now % COLOR_COUNT);
		}
		HLT(); // halt until timer irq wakes us up
	}
}
