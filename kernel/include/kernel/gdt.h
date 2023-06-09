#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// A GDT entry is structured as follows:
// |base 24:31|flags 0:3|limit 16:19|access 0:7|base 16:23|base 0:15|limit 0:15|
// where `access` is |P|DPL 0:1|S|Ex|DC|RW|Ac|
// and `flags` is |Granularity|Size|
typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t limit_and_flags;
	uint8_t base_high;
} __attribute__ ((packed)) gdt_entry_t;

typedef struct {
	uint16_t size;
	uint32_t offset;
} __attribute__ ((packed)) gdt_pointer_t;

typedef struct {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldtr;
	uint16_t trap;
	uint16_t iomap_base;
    uint32_t ssp;
} __attribute__ ((packed)) tss_entry_t;

// Segment Selector
//  format: [ 15                        3 | 2| 1 0 ]
//          [           13-bits index     |TI| RPL ]
#define SEG_SELECTOR_REQUESTED_PRIV(x)        (((x) &  0x03))   // RPL Set privilege level (0 - 3)
#define SEG_SELECTOR_TI(x)      				((x) << 1) // specifies which descriptor table to use. If clear (0) then the GDT is used, if set (1) then the current LDT is used.


void gdt_init();
void gdt_set_entry(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void gdt_write_tss(uint32_t num, uint32_t ss0, uint32_t esp0);
void gdt_set_kernel_stack(uintptr_t stack);

extern void gdt_load(gdt_pointer_t* gdt_ptr);

extern void tss_load(uintptr_t gdt_ptr);

#endif
