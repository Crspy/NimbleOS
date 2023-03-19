#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>
#include <kernel/isr.h>

typedef union
{
    uint32_t raw_val;
    struct
    {
        uint32_t present : 1;
        uint32_t rw : 1;
        uint32_t user : 1;
        uint32_t writethrough : 1;
        uint32_t disable_cache : 1;
        uint32_t accessed : 1;
        uint32_t zero : 1;
        uint32_t large_pages : 1;
        uint32_t unused : 1;
        uint32_t available : 3;
        uint32_t frame : 20;
    };
} __attribute__((packed)) directory_entry_t;

typedef union
{
    uint32_t raw_val;
    struct
    {
        uint32_t present : 1;
        uint32_t rw : 1;
        uint32_t user : 1;
        uint32_t writethrough : 1;
        uint32_t disable_cache : 1;
        uint32_t accessed : 1;
        uint32_t dirty : 1;
        uint32_t zero : 1;
        uint32_t global : 1;
        uint32_t available : 3;
        uint32_t frame : 20;
    };
} __attribute__((packed)) page_entry_t;

void paging_init();
void paging_map_page(uintptr_t virt, uintptr_t phys, uint32_t flags);
page_entry_t *paging_get_page(uintptr_t virt, bool create);
void paging_unmap_page(uintptr_t virt);
void paging_map_memory(uintptr_t virt,uintptr_t phys, uint32_t size);
void paging_switch_directory(uintptr_t dir_phys);
void paging_invalidate_cache();
void paging_invalidate_page(uintptr_t virt);
void paging_fault_handler(registers_t *regs);

#define KERNEL_BASE_VIRT 0xC0000000
#define P2V(addr) ((uintptr_t)(addr) + KERNEL_BASE_VIRT)
#define V2P(addr) ((uintptr_t)(addr)-KERNEL_BASE_VIRT)

#define PAGE_PRESENT 1
#define PAGE_RW 2
#define PAGE_USER 4

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 1024

#endif
