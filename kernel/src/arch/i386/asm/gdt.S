#include <kernel/cpu.h>
.section .text
.align 4

.global gdt_load
.type gdt_load, @function

gdt_load:
	mov 4(%esp), %eax
	lgdt (%eax)

	mov $GDT_SELECTOR_DATA0, %ax  
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $GDT_SELECTOR_CODE0, $load 
load:
	ret