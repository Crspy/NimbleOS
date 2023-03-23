#include <kernel/cpu.h>
.section .text
.align 4

.global gdt_load
.type gdt_load, @function

gdt_load:
	mov 4(%esp), %eax
	lgdt (%eax)

	# Reload segment registers
	mov $GDT_SELECTOR_KERNEL_DATA, %ax  
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $GDT_SELECTOR_KERNEL_CODE, $load # reload 'cs' segment
load:
	ret

.global tss_load
.type tss_load, @function
tss_load:
	mov 4(%esp), %eax
	ltr %ax
	ret