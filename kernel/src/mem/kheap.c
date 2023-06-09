#include <kernel/paging.h>
#include <kernel/kheap.h>

static uint8_t* heap_pointer = (uint8_t*)KERNEL_HEAP_BASE_VIRT;

/* Used to allocate memory for use by the kernel.
 * The memory is pre-mapped, which means clones of the kernel page directory
 * (i.e processes) share their kernel memory in kernel mode, for instance
 * during syscalls.
 * There's no corresponding kfree. What the kernel takes, the kernel keeps.
 */
void* kmalloc(uint32_t size) {
	if ((uintptr_t)heap_pointer + size >= KERNEL_HEAP_END_VIRT) {
		return NULL;
	}

	uint8_t* previous_heap = heap_pointer;
	heap_pointer += size;

	return previous_heap;
}

/* Aligned memory allocator.
 * Returns `size` bytes of memory at an address multiple of `align`.
 */
void* kamalloc(uint32_t size, uint32_t align) {
	uintptr_t next = (((uintptr_t)heap_pointer / align) + 1) * align;

	if (next + size >= KERNEL_HEAP_END_VIRT) {
		return NULL;
	}

	heap_pointer = (uint8_t*)(next + size);

	return (uint8_t*)next;
}