#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/cpu.h>
#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/pmm.h>
#include <kernel/paging.h>
#include <assert.h>

extern uint32_t KERNEL_BEGIN;
extern uint32_t KERNEL_BEGIN_PHYS;
extern uint32_t KERNEL_END;
extern uint32_t KERNEL_END_PHYS;
extern uint32_t KERNEL_SIZE;

void kernel_main(multiboot_info_t* boot, uint32_t magic) {
	term_init();
	gdt_init();
	idt_init();
	irq_init();
	timer_init();
	printf("Welcome to \x1B[33mNimble OS\x1B[37m v1.0 !\n\n");

	printf("Kernel loaded at 0x%X, ending at 0x%X (%dKiB)\n", &KERNEL_BEGIN_PHYS, &KERNEL_END_PHYS,
		((uint32_t) &KERNEL_SIZE)/1024);

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printf("Not a multiboot compliant bootloader... Let's try anyway\n");
		abort();
	}
	
	pmm_init(boot->mem_lower + boot->mem_upper);

	if (boot->flags & MULTIBOOT_INFO_MEM_MAP) {
		// I didn't even know these worked in protected mode
		// Looking at the assembly, I think they just take up more space,
		// but they're still processed by 32-bits registers
		uint64_t available = 0;
		uint64_t unavailable = 0;
		multiboot_memory_map_t* mmap = (multiboot_memory_map_t*) P2V(boot->mmap_addr);

		while ((uintptr_t) mmap < (P2V(boot->mmap_addr)+boot->mmap_length)) {
			if (!mmap->len) {
				continue;
			}

			if (mmap->type == 1) {
				pmm_init_region((uintptr_t) mmap->addr, mmap->len);
				available += mmap->len;
			}
			else {
				unavailable += mmap->len;
			}

			// Casts needed to get around pointer increment magic
			mmap = (multiboot_memory_map_t*) ((uintptr_t) mmap + mmap->size + sizeof(uintptr_t));
		}

		printf("Total available memory: \x1B[32m%dMiB\x1B[37m\n", available >> 20);
		printf("Total unavailable memory: \x1B[32m%dKiB\x1B[37m\n", unavailable >> 10);
	}
	else {
		printf("ERROR: No memory map given, catching fire...\n");
		abort();
	}

	// Protect low memory, kernel and the PMM itself
	pmm_deinit_region((uintptr_t) 0, (uint32_t) &KERNEL_END_PHYS + pmm_get_map_size());

	paging_init();
	

	uint32_t time = 0;
	while (1) {
		uint32_t ntime = (uint32_t) timer_get_time();
		if (ntime > time) {
			time = ntime;
			printf("\x1B[s\x1B[24;0H"); // save & move cursor
			printf("\x1B[K");            // Clear line
			printf("%ds", time);
			printf("\x1B[u");            // Restore cursor
		}
		HLT();
	}
}

