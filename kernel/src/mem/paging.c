#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/paging.h>
#include <kernel/pmm.h>
#include <kernel/isr.h>
#include <kernel/tty.h>
#include <kernel/kheap.h>

#define DIRECTORY_INDEX(x) ((x) >> 22)
#define TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_OFFSET(x) ((x) & 0xFFF)

static directory_entry_t* current_page_directory;
extern directory_entry_t kernel_directory[PAGE_ENTRIES];



void paging_init() {
	isr_register_handler(14, &paging_fault_handler);

	// Setup the recursive page directory entry
	uintptr_t dir_phys = V2P(&kernel_directory);
	kernel_directory[PAGE_ENTRIES - 1].raw_val = dir_phys | PAGE_PRESENT | PAGE_RW; // 0xFFC00000 >> 22 = 1023 (map dir_phys to that virtual address)
	paging_invalidate_page(0xFFC00000);


	// in boot.S we already identity mapped the first 4MB using big size flag
	// so we "undo" that here because we only need to identity map the 1st MB
	kernel_directory[0].raw_val = 0;
	paging_invalidate_page(0x00000000); // let it take effect

	paging_map_pages(0x00000000, 0x00000000, 256, PAGE_RW); // Identity map only the 1st MB
	current_page_directory = kernel_directory;

	// Map the kernel heap
	uint32_t heap_pages = (KERNEL_HEAP_END_VIRT - KERNEL_HEAP_BASE_VIRT) / PAGE_SIZE;
	uintptr_t heap_phys = pmm_alloc_pages(heap_pages);
	paging_map_pages(KERNEL_HEAP_BASE_VIRT, heap_phys, heap_pages, PAGE_RW);


	// Map tty VGA memory
	paging_map_page(KERNEL_VGA_BASE_VIRT,VGA_MEMORY,PAGE_RW);
	term_set_buffer((uint16_t*)KERNEL_VGA_BASE_VIRT);
}

uintptr_t paging_get_kernel_directory() {
	return V2P(&kernel_directory);
}

/* Given a page-aligned virtual address, returns a pointer to the corresponding
 * page table entry. Usually, this entry is then filled with appropriate page
 * information such as the physical address it points to, whether it is writable
 * etc...
 * If the `create` flag is passed, the corresponding page table is created with
 * the passed flags if needed and this function should never return NULL.
 */
page_entry_t* paging_get_page(uintptr_t virt, bool create, uint32_t flags) {
	if (virt % PAGE_SIZE) {
		printf("[VMM] Tried to access a page at an unaligned address!\n");
		abort();
	}

	uint32_t dir_index = DIRECTORY_INDEX(virt);
	uint32_t table_index = TABLE_INDEX(virt);

	directory_entry_t* dir = (directory_entry_t*)0xFFFFF000; // references last entry in page_directory which is self referencing
	page_entry_t* table = ((page_entry_t*)0xFFC00000) + (PAGE_ENTRIES * dir_index);

	if (!dir[dir_index].present && create) {
		page_entry_t* new_table = (page_entry_t*)pmm_alloc_page();
		dir[dir_index].raw_val = (uint32_t)new_table | PAGE_PRESENT | PAGE_RW  | (flags & PAGE_FLAGS);
		memset((void*)table, 0x00, PAGE_SIZE);
	}



	if (dir[dir_index].present) {
		return &table[table_index];
	}

	return NULL;
}

// TODO: refuse 4 MB pages
void paging_map_page(uintptr_t virt, uintptr_t phys, uint32_t flags) {
	page_entry_t* page = paging_get_page(virt, true , flags);

	if (page->present) {
		printf("[VMM] Tried to map an already mapped virtual address 0x%X to 0x%X\n",
			virt, phys);
		printf("[VMM] Previous mapping: 0x%X to 0x%X\n", virt, page->raw_val & PAGE_FRAME);
		abort();
	}

	page->raw_val = phys | PAGE_PRESENT | (flags & PAGE_FLAGS);
	paging_invalidate_page(virt);
}

void paging_map_pages(uintptr_t virt, uintptr_t phys, uint32_t num, uint32_t flags) {
	for (uint32_t i = 0; i < num; i++) {
		paging_map_page(virt, phys, flags);
		phys += PAGE_SIZE;
		virt += PAGE_SIZE;
	}
}


void paging_unmap_page(uintptr_t virt) {
	page_entry_t* page = paging_get_page(virt, false, 0);

	if (page) {

		page->present = 0;
		paging_invalidate_page(virt);
	}
	else
	{
		printf("[VMM] Tried to unmap a page that doesn't exist with virt address %p\n", virt);
	}
}

void paging_unmap_pages(uintptr_t virt, uint32_t num) {
	for (uint32_t i = 0; i < num; i++) {
		paging_unmap_page(virt);
		virt += PAGE_SIZE;
	}
}

void paging_switch_directory(uintptr_t dir_phys) {
	asm volatile("mov %0, %%cr3\n" :: "r"(dir_phys));
	current_page_directory = (directory_entry_t*)P2V(dir_phys);
}

void paging_invalidate_cache() {
	asm volatile(
		"mov %cr3, %eax\n"
		"mov %eax, %cr3\n");
}

void paging_invalidate_page(uintptr_t virt) {
	asm volatile("invlpg (%0)" ::"r" (virt) : "memory");
}

void paging_fault_handler(registers_t* regs) {
	uint32_t err = regs->err_code;
	uintptr_t cr2 = 0;
	asm volatile("mov %%cr2, %0\n" : "=r"(cr2));

	printf("\x1B[37;44m");
	printf("The page at 0x%X %s present ", cr2, err & 0x01 ? "was" : "wasn't");
	printf("when a process tried to %s it.\n", err & 0x02 ? "write to" : "read from");
	printf("This process was in %s mode.\n", err & 0x04 ? "user" : "kernel");

	page_entry_t* page = paging_get_page(cr2 & PAGE_FRAME, false, 0);

	if (page) {
		printf("The page was in %s mode.\n", page->user ? "user" : "kernel");
	}

	printf("The reserved bits %s overwritten.\n", err & 0x08 ? "were" : "weren't");
	printf("The fault %s during an instruction fetch.\n", err & 0x10 ? "occured" : "didn't occur");

	abort();
}

uintptr_t paging_virt_to_phys(uintptr_t virt) {
	page_entry_t* p = paging_get_page(virt & PAGE_FRAME, false , 0);

	if (!p)
		return 0;

	return (p->raw_val & PAGE_FRAME) + PAGE_OFFSET(virt);
}


