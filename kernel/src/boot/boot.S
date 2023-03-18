.set MAGIC,				0x1BADB002
.set FLAGS_PAGE_ALIGN,	1 << 0
.set FLAGS_MEMORY,		1 << 1
.set FLAGS_GRAPHICS,	1 << 2
.set FLAGS,				FLAGS_PAGE_ALIGN | FLAGS_MEMORY /*| FLAGS_GRAPHICS*/
.set CHECKSUM,			-(MAGIC + FLAGS)

# Declare a header as in the Multiboot Standard.
.section .multiboot
	.align 4
	/* The magic fields of Multiboot header */
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	/* The address fields of Multiboot header */
	.long 0x00000000 /* header_addr */
	.long 0x00000000 /* load_addr */
	.long 0x00000000 /* load_end_addr */
	.long 0x00000000 /* bss_end_addr */
	.long 0x00000000 /* entry_addr */
	/* The graphics fields of Multiboot header */
	.long 0		/* mode_type */
	.long 0 	/* width */
	.long 0 	/* height */
	.long 0 	/* depth */

# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
	.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	# Save %eax that contains the magic number
	pushl %eax
	# Save %ebx that contains the multiboot header address
	pushl %ebx

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	# Pass the multiboot header adress and magic number.
	# See https://www.gnu.org/software/grub/manual/multiboot/html_node/Machine-state.html
	# pushl %eax	# (magic number) this was already pushed previously
	# pushl %ebx	# (multiboot header address) this was already pushed previously
	cli
	call kernel_main
	add $12, %esp # cleanup the stack. useless here

	# Call the global destructors
	call _fini

	# Hang if kernel_main unexpectedly returns.
	cli
.Lhang:
	hlt
	jmp .Lhang

.size _start, . - _start
