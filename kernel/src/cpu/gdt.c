#include <stdint.h>
#include <string.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/cpu.h>


// access bits
#define SEG_LONG(x)      ((x) << 1 << 4) // Long mode
#define SEG_SIZE(x)      ((x) << 2 << 4) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 3 << 4) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)

// flags bits
#define SEG_PRES(x)        ((x) << 7) // Present
#define SEG_PRIV(x)        (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
#define SEG_DESCTYPE(x)  ((x) << 4) // Descriptor type (0 for system, 1 for code/data)
#define SEG_EXEC(x)        ((x) << 3) // Executable bit (0 for data , 1 for code)
#define SEG_CONFORM(x)        ((x) << 2) // Executable bit (0 for data , 1 for code)
#define SEG_RW(x)        ((x) << 1) // Readable bit for code segment , Writable bit for data segment
#define SEG_ACCESS(x)        ((x) << 0) // Best left clear (0), the CPU will set it when the segment is accessed.


#define GDT_SEG_FLAGS	SEG_LONG(0)  | SEG_SIZE(1) | SEG_GRAN(1)  // 0xC0

#define GDT_CODE_PL0_ACCESS	SEG_ACCESS(0) | SEG_RW(1) | SEG_CONFORM(0) | \
							SEG_EXEC(1) | SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_PRIV(0)  // 0x9A

#define GDT_DATA_PL0_ACCESS	SEG_ACCESS(0) | SEG_RW(1) | SEG_CONFORM(0) | \
							SEG_EXEC(0) | SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_PRIV(0)  // 0x92

#define GDT_CODE_PL3_ACCESS	SEG_ACCESS(0) | SEG_RW(1) | SEG_CONFORM(0) | \
							SEG_EXEC(1) | SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_PRIV(3)  // 0xFA

#define GDT_DATA_PL3_ACCESS	SEG_ACCESS(0) | SEG_RW(1) | SEG_CONFORM(0) | \
							SEG_EXEC(0) | SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_PRIV(3)  // 0xF2

#define GDT_TSS_ACCESS	SEG_ACCESS(1) | SEG_RW(0) | SEG_CONFORM(0) | \
							SEG_EXEC(1) | SEG_DESCTYPE(0) | SEG_PRES(1) | SEG_PRIV(3)  // 0xE9


gdt_entry_t gdt_entries[6];
gdt_entry_ptr_t gdt_ptr;

static tss_entry_t tss;

void gdt_init() {

	gdt_ptr.limit = sizeof(gdt_entries) - 1;
	gdt_ptr.base = (uint32_t)&gdt_entries;

	gdt_set_entry(0, 0, 0, 0, 0);  // Null segment
	gdt_set_entry(1, 0, 0xFFFFFFFF, GDT_CODE_PL0_ACCESS, GDT_SEG_FLAGS); // Code segment
	gdt_set_entry(2, 0, 0xFFFFFFFF, GDT_DATA_PL0_ACCESS, GDT_SEG_FLAGS); // Data segment
	gdt_set_entry(3, 0, 0xFFFFFFFF, GDT_CODE_PL3_ACCESS, GDT_SEG_FLAGS); // User mode code segment
	gdt_set_entry(4, 0, 0xFFFFFFFF, GDT_DATA_PL3_ACCESS, GDT_SEG_FLAGS); // User mode data segment
	gdt_write_tss(5, GDT_SELECTOR_DATA0, 0x00);

	gdt_load((uintptr_t)&gdt_ptr);
	// Flush the TSS
	tss_load(GDT_SELECTOR_TSS | SEG_SELECTOR_REQUESTED_PRIV(3));
}

void gdt_set_entry(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
	gdt_entries[num].base_low = (base & 0xFFFF);
	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
	gdt_entries[num].base_high = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low = (limit & 0xFFFF);
	gdt_entries[num].granularity = (limit >> 16) & 0x0F;

	gdt_entries[num].granularity |= flags & 0xF0;
	gdt_entries[num].access = access;
}

void gdt_write_tss(uint32_t num, uint32_t ss0, uint32_t esp0) {
	uintptr_t base = (uintptr_t)&tss;
	uintptr_t limit = base + sizeof(tss);

	/* Add the TSS descriptor to the GDT */
	gdt_set_entry(num, base, limit, GDT_TSS_ACCESS, 0x00);

	memset(&tss, 0x00, sizeof(tss));

	tss.ss0 = ss0;
	tss.esp0 = esp0;
	tss.cs = GDT_SELECTOR_CODE0 | SEG_SELECTOR_REQUESTED_PRIV(3);
	tss.ss = GDT_SELECTOR_DATA0 | SEG_SELECTOR_REQUESTED_PRIV(3);
	tss.ds = GDT_SELECTOR_DATA0 | SEG_SELECTOR_REQUESTED_PRIV(3);
	tss.es = GDT_SELECTOR_DATA0 | SEG_SELECTOR_REQUESTED_PRIV(3);
	tss.fs = GDT_SELECTOR_DATA0 | SEG_SELECTOR_REQUESTED_PRIV(3);
	tss.gs = GDT_SELECTOR_DATA0 | SEG_SELECTOR_REQUESTED_PRIV(3);

	tss.iomap_base = sizeof(tss);
}

void gdt_set_kernel_stack(uintptr_t stack) {
	tss.esp0 = stack;
}