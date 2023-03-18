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

void kernel_main(multiboot_info_t *mbi, uint32_t magic)
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

	printf("Welcome to \x1B[0;33mNimble OS\x1B[37m v1.0 !\n\n");


	if (mbi->flags & MULTIBOOT_INFO_MEM_MAP)
	{
		// I didn't even know these worked in protected mode
		// Looking at the assembly, I think they just take up more space,
		// but they're still processed by 32-bits registers
		uint64_t available = 0;
		uint64_t unavailable = 0;
		multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mbi->mmap_addr;

		for (mmap = (multiboot_memory_map_t *)mbi->mmap_addr;
			 (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
			 mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
		{
			if (mmap->type == 1)
			{
				available += mmap->len;
			}
			else
			{
				unavailable += mmap->len;
			}
		}

		printf("Total available memory: \x1B[32m%dMiB\x1B[37m\n", available >> 20);
		printf("Total unavailable memory: \x1B[32m%dKiB\x1B[37m\n", unavailable >> 10);
	}
	else
	{
		printf("ERROR: No memory map given, catching fire...\n");
		abort();
	}

	uint32_t time_start = timer_get_time();
	while (1)
	{
		uint32_t time_now = timer_get_time();
		if (time_now != time_start)
		{
			time_start = time_now;
			printf("time: %d\n", time_now); // will print every 1s
		}
		HLT(); // halt until timer irq wakes us up
	}
}
