#include <kernel/cpu.h>
.section .text
.align 4

.macro ISR_NOERR num
	.global isr\num
	isr\num:
		cli
		push $0
		push $\num
		jmp isr_common_handler
.endm

.macro ISR_ERR num
	.global isr\num
	isr\num:
		cli
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
ISR_NOERR 17
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
ISR_NOERR 30
ISR_NOERR 31

.extern isr_handler
.type isr_handler, @function

isr_common_handler:
	pushal       

	mov %ds, %ax   # save data segment selector
	push %eax

	mov $GDT_SELECTOR_DATA0, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	push %esp      # to access all registers from isr_handler
	call isr_handler
	add $4, %esp

	pop %eax      # restore data segment selector
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popal
	add $8, %esp # clean up the pushed arguments (err code , isr no)
	iret
