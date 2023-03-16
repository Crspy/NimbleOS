#include <stdint.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>


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


gdt_entry_t gdt_entries[5];
gdt_entry_ptr_t gdt_ptr;
idt_entry_t idt_entries[256];
idt_entry_ptr_t idt_entry_ptr;

void init_gdt() {
	
	gdt_ptr.limit = (sizeof(gdt_entry_t)*5) - 1;
	gdt_ptr.base = (uint32_t) &gdt_entries;

	gdt_set_entry(0, 0, 0, 0, 0);  // Null segment
	gdt_set_entry(1, 0, 0xFFFFFFFF, GDT_CODE_PL0_ACCESS, GDT_SEG_FLAGS); // Code segment
	gdt_set_entry(2, 0, 0xFFFFFFFF, GDT_DATA_PL0_ACCESS, GDT_SEG_FLAGS); // Data segment
	gdt_set_entry(3, 0, 0xFFFFFFFF, GDT_CODE_PL3_ACCESS, GDT_SEG_FLAGS); // User mode code segment
	gdt_set_entry(4, 0, 0xFFFFFFFF, GDT_DATA_PL3_ACCESS, GDT_SEG_FLAGS); // User mode data segment

	gdt_load((uint32_t) &gdt_ptr);
}

void gdt_set_entry(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
	gdt_entries[num].base_low = (base & 0xFFFF);
	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
	gdt_entries[num].base_high = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low = (limit & 0xFFFF);
	gdt_entries[num].granularity = (limit >> 16) & 0x0F;

	gdt_entries[num].granularity |= flags & 0xF0;
	gdt_entries[num].access	= access;
}
