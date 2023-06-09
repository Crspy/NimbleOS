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
#include <kernel/proc.h>

extern uint32_t KERNEL_BEGIN_PHYS;
extern uint32_t KERNEL_END_PHYS;
extern uint32_t KERNEL_SIZE;


void process_1()
{
	while (true)
	{
		asm volatile(
			"mov $2022,%ecx\n"
			"push %ecx\n"
			"pop %ecx");
	}

}
void process_2()
{
	while (true)
	{
		asm volatile(
			"mov $1414,%ecx\n"

			);
	}
}

void process_3()
{
	asm volatile(
		"mov $3, %eax\n" // putchar
		"mov $'H', %ebx\n" // H
		"int $0x30\n"
		"mov $'e', %ebx\n" // e
		"int $0x30\n"
		"mov $'l', %ebx\n" // l
		"int $0x30\n"
		"mov $'l', %ebx\n" // l
		"int $0x30\n"
		"mov $'o', %ebx\n" // o
		"int $0x30\n"
		"mov $',', %ebx\n" // ,
		"int $0x30\n"
		"mov $' ', %ebx\n" //  ' '
		"int $0x30\n"
		"mov $'w', %ebx\n" // w
		"int $0x30\n"
		"mov $'o', %ebx\n" // o
		"int $0x30\n"
		"mov $'r', %ebx\n" // r
		"int $0x30\n"
		"mov $'l', %ebx\n" // l
		"int $0x30\n"
		"mov $'d', %ebx\n" // d
		"int $0x30\n"
		"mov $'\n', %ebx\n" // '\n'
		"int $0x30\n"

		// syscall_wait 10 seconds (not working yet)
		"mov $2, %eax\n"
		"mov $10, %ebx\n"
		"int $0x30\n"

		"mov $1, %eax\n"  // # process exit
		"int $0x30\n"

		);
}

void kernel_main(multiboot_info_t* mbi, uint32_t magic)
{

	term_init();
	printf("Welcome to \x1B[33mNimble OS\x1B[37m v1.0 !\n\n");

	printf("Kernel loaded at 0x%X, ending at 0x%X (%dKB)\n", &KERNEL_BEGIN_PHYS, &KERNEL_END_PHYS,
		((uint32_t)&KERNEL_SIZE) / 1024);

	assert(magic == MULTIBOOT_BOOTLOADER_MAGIC);
	assert(mbi->flags & MULTIBOOT_INFO_MEM_MAP);

	gdt_init();
	idt_init();
	isr_init();
	irq_init();
	timer_init();
	keyboard_init();
	pmm_init(mbi);
	paging_init();
	syscall_init();
	


	proc_run_code(process_1, 128);
	proc_run_code(process_2, 128);
	proc_run_code(process_3, 128);
	proc_print_processes();

	proc_init();
	printf("bruh\n");

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
