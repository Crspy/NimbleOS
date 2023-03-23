#include <stdio.h>
#include <stddef.h>

#include <kernel/syscall.h>
#include <kernel/proc.h>
#include <kernel/isr.h>
#include <kernel/sys.h>


static void syscall_handler(registers_t* regs);
static void syscall_yield(registers_t* regs);
static void syscall_exit(registers_t* regs);
static void syscall_wait(registers_t* regs);
static void syscall_putchar(registers_t* regs);

sys_handler_t syscall_handlers[SYSCALL_COUNT] = { NULL };

void syscall_init() {
	isr_register_handler(48, &syscall_handler);
	syscall_handlers[0] = syscall_yield;
	syscall_handlers[1] = syscall_exit;
	syscall_handlers[2] = syscall_wait;
	syscall_handlers[3] = syscall_putchar;
}

static void syscall_handler(registers_t* regs) {
	//printf("syscall %d\n", regs->eax);
	if (syscall_handlers[regs->eax]) {

		sys_handler_t handler = syscall_handlers[regs->eax];
		handler(regs);
	}
	else {
		printf("Unknown syscall %d\n", regs->eax);
	}
}

static void syscall_yield(registers_t* regs) {
	proc_switch_process(regs);
}

static void syscall_exit(registers_t* regs) {
	UNUSED_PARAM(regs);

	proc_exit_current_process();
}

static void syscall_wait(registers_t* regs) {
	// TODO
	/* This must be implemented by the scheduler, not here:
	 * - IRQs don't fire while in a syscall, so we can't rely on the timer
	 *   increasing
	 * - We can't both task switch and come back to this handler to check
	 *   the time
	 */
}

static void syscall_putchar(registers_t* regs) {
	putchar((char)regs->ebx);
}