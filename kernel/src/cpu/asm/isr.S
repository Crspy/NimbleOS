#include <kernel/cpu.h>
.section .text
.align 4

# Only interrupts 8, 10, 11, 12, 13, 14 and 17 push an error code, so we
# compensate by pushing 0 for other interrupts.
# This allows reusing the same structure for all interrupts in kernel code.
# Note: interrupts are disabled on call, see the flags in `idt.h`
# Note: when called, the calling `eflags`, `cs` and `eip` are already on the stack
.macro ISR_NOERR num
	.global isr\num
	isr\num:
		push $0
		push $\num
		jmp isr_common_handler
.endm

.macro ISR_ERR num
	.global isr\num
	isr\num:
		push $\num
		jmp isr_common_handler
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR 30
ISR_NOERR 31
ISR_NOERR 48 # syscall

.extern isr_handler # void isr_handler(registers_t* regs)
.type isr_handler, @function

isr_common_handler:
	# Save and push registers and data segments
	pushal       

	push %ds
	push %es
	push %fs
	push %gs

	# Load kernel data segments (cs is already loaded by the interrupt)
	# in case we were running in usermode
	mov $GDT_SELECTOR_KERNEL_DATA, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	push %esp      # (`registers_t` pointer) to access all registers from isr_handler
	call isr_handler
	add $4, %esp

	# Restore registers and data segments
	pop %ds
	pop %es
	pop %fs
	pop %gs

	popal
	add $8, %esp # clean up the pushed arguments (err code , isr no)

	# Return to whatever was interrupted
	# This will load the previous code segment, `eip` and `eflags`
	iret
