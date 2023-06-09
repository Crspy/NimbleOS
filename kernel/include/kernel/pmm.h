#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <kernel/multiboot.h>

void pmm_init(multiboot_info_t* mbi);
uint32_t pmm_get_map_size();
void pmm_init_region(uintptr_t addr, uint32_t size);
void pmm_deinit_region(uintptr_t addr, uint32_t size);
uintptr_t pmm_alloc_page();
uintptr_t pmm_alloc_aligned_large_page();
uintptr_t pmm_alloc_pages(uint32_t num);
void pmm_free_page(uintptr_t addr);
void pmm_free_pages(uintptr_t addr, uint32_t num);
void mmap_set(uint32_t bit);
void mmap_unset(uint32_t bit);
uint32_t mmap_test(uint32_t bit);
uint32_t mmap_find_free();
uint32_t mmap_find_free_frame(uint32_t num);

extern uint32_t* mem_map;

#define PMM_BLOCK_SIZE 4096

#endif
