#include <kernel/paging.h>
#include <kernel/pmm.h>
#include <string.h>
#include <kernel/isr.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DIRECTORY_INDEX(x) ((x) >> 22)
#define TABLE_INDEX(x) (((x) >> 12) & 0x3FF)

static directory_entry_t* current_page_directory;
extern directory_entry_t kernel_directory[PAGE_ENTRIES];

void paging_init() {
	isr_register_handler(14, &paging_fault_handler);

	// Setup the recursive page directory entry
	uintptr_t dir_phys = V2P((uintptr_t) &kernel_directory);
	kernel_directory[PAGE_ENTRIES-1].raw_val = dir_phys | PAGE_PRESENT | PAGE_RW; // 0xFFC00000 >> 22 = 1023 (map dir_phys to that virtual address)
	paging_invalidate_page(0xFFC00000);
}

// TODO: refuse 4 MiB pages
void paging_map_page(uintptr_t virt, uintptr_t phys, uint32_t flags) {
	page_entry_t* page = paging_get_page(virt, true);

	if (page->present) {
		printf("[VMM] Tried to map an already mapped virtual address!\n");
		abort();
	}
 
    page->raw_val = phys | PAGE_PRESENT | (flags & 0xFFF);
	paging_invalidate_page(virt);
}

page_entry_t* paging_get_page(uintptr_t virt, bool create) {
	if (virt % PAGE_SIZE) {
		printf("[VMM] Tried to access a page at an unaligned address!\n");
		abort();
	}

	uint32_t dir_index = DIRECTORY_INDEX(virt);
	uint32_t table_index = TABLE_INDEX(virt);

	directory_entry_t* dir = (directory_entry_t*) 0xFFFFF000; // references last entry in page_directory which is self referencing
	page_entry_t* table = ((page_entry_t*) 0xFFC00000) + (PAGE_ENTRIES * dir_index);

	if (!(dir[dir_index].present && create)) {
		page_entry_t* new_table = (page_entry_t*) pmm_alloc_page();
		dir[dir_index].raw_val = (uint32_t) new_table | PAGE_PRESENT | PAGE_RW;
		memset((void*) table, 0x00, PAGE_SIZE);
	}
	else {
		return 0;
	}

	return &table[table_index];
}

void paging_unmap_page(uintptr_t virt) {
	page_entry_t* page = paging_get_page(virt, false);

	if (page) {
		page->present = 0;
		paging_invalidate_page(virt);
	}
}

void paging_map_memory(uintptr_t virt,uintptr_t phys, uint32_t size) {
	// size / PAGE_SIZE = number of pages to map
	for (uint32_t i = 0; i < size / PAGE_SIZE; i++) {
		paging_map_page(virt,phys, PAGE_RW);
		phys += PAGE_SIZE;
		virt += PAGE_SIZE;
	}
}

void paging_switch_directory(uintptr_t dir_phys) {
	asm volatile("mov %0, %%cr3\n" :: "r"(dir_phys));
	current_page_directory = (directory_entry_t*) P2V(dir_phys);
}

void paging_invalidate_cache() {
	asm volatile(
		"mov %cr3, %eax\n"
		"mov %eax, %cr3\n");
}

void paging_invalidate_page(uintptr_t virt) {
    asm volatile ("invlpg (%0)" :: "b"(virt) : "memory");
}

void paging_fault_handler(registers_t* regs) {
	uint32_t err = regs->err_code;

	printf("\x1B[37;44m");
	printf("The page requested %s present ", err & 0x01 ? "was" : "wasn't");
	printf("when a process tried to %s it.\n", err & 0x02 ? "write to" : "read from");
	printf("This process was in %s mode.\n", err & 0x04 ? "user" : "kernel");
	printf("The reserved bits %s overwritten.\n", err & 0x08 ? "were" : "weren't");
	printf("The fault %s during an instruction fetch.\n", err & 0x10 ? "occured" : "didn't occur");

	abort();
}
