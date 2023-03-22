#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <kernel/cpu.h>
#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/keyboard.h>
#include <kernel/pmm.h>
#include <kernel/paging.h>
#include <kernel/syscall.h>

extern uint32_t KERNEL_BEGIN_PHYS;
extern uint32_t KERNEL_END_PHYS;
extern uint32_t KERNEL_SIZE;

void kernel_main(multiboot_info_t *mbi, uint32_t magic)
{
	term_init();
	printf("Welcome to \x1B[33mNimble OS\x1B[37m v1.0 !\n\n");

	printf("Kernel loaded at 0x%X, ending at 0x%X (%dKiB)\n", &KERNEL_BEGIN_PHYS, &KERNEL_END_PHYS,
		   ((uint32_t)&KERNEL_SIZE) / 1024);

	assert(magic == MULTIBOOT_BOOTLOADER_MAGIC);
	assert(mbi->flags & MULTIBOOT_INFO_MEM_MAP);

	gdt_init();
	idt_init();
	irq_init();
	timer_init();
	keyboard_init();
	pmm_init(mbi);
	paging_init();
	syscall_init();

	uint32_t time = 0;
	while (1)
	{
		uint32_t ntime = (uint32_t)timer_get_time();
		if (ntime > time)
		{
			time = ntime;
			printf("\x1B[s\x1B[24;0H"); // save & move cursor
			printf("\x1B[K");			// Clear line
			printf("%ds", time);
			printf("\x1B[u"); // Restore cursor
		}
		HLT();
	}
}
