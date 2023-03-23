#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
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
uintptr_t paging_get_kernel_directory();
page_entry_t* paging_get_page(uintptr_t virt, bool create, uint32_t flags);
void paging_map_page(uintptr_t virt, uintptr_t phys, uint32_t flags);
void paging_map_pages(uintptr_t virt, uintptr_t phys, uint32_t num, uint32_t flags);
void paging_unmap_page(uintptr_t virt);
void paging_unmap_pages(uintptr_t virt, uint32_t num);

void paging_switch_directory(uintptr_t dir_phys);
void paging_invalidate_cache();
void paging_invalidate_page(uintptr_t virt);
void paging_fault_handler(registers_t* regs);

uintptr_t paging_virt_to_phys(uintptr_t virt);


#define KERNEL_BASE_VIRT 0xC0000000

/* We assume here that the kernel code data and stack as setup by our linker
 * script ends before addresses 0xD0000000, which is a fair guess, as long as
 * our kernel occupies less than 255 MiB of memory.
 * We could also use the `KERNEl_END_PHYS` linker symbol to avoid making these
 * assumptions, and probably to gain address space, but having clear separations
 * has advantages in debugging.
 */
 // 4 MB of heap space
#define KERNEL_HEAP_BASE_VIRT 0xD0000000
#define KERNEL_HEAP_END_VIRT 0xD0400000 
#define KERNEL_VGA_BASE_VIRT (KERNEL_HEAP_BASE_VIRT - PAGE_SIZE)

#define P2V(addr) ((uintptr_t)(addr) + KERNEL_BASE_VIRT)
#define V2P(addr) ((uintptr_t)(addr) - KERNEL_BASE_VIRT)

#define PAGE_PRESENT (1 << 0)
#define PAGE_RW (1 << 1)
#define PAGE_USER (1 << 2)
#define PAGE_LARGE   (1 << 7)
#define PAGE_FRAME   0xFFFFF000
#define PAGE_FLAGS   0x00000FFF

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 1024

#endif
