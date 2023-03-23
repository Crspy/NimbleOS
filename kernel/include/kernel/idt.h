#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// flags bits
#define IDT_ENTRY_PRES(x)        ((x) << 7) // Present
#define IDT_ENTRY_PRIV(x)        (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
#define IDT_ENTRY_TYPE_TASK_GATE 0x5
#define IDT_ENTRY_TYPE_16BIT_INTERRUPT_GATE 0x6
#define IDT_ENTRY_TYPE_16BIT_TRAP_GATE 0x7
#define IDT_ENTRY_TYPE_32BIT_INTERRUPT_GATE 0xE
#define IDT_ENTRY_TYPE_32BIT_TRAP_GATE 0xF

#define IDT_INT_KERNEL IDT_ENTRY_PRES(1) | IDT_ENTRY_PRIV(0) | IDT_ENTRY_TYPE_32BIT_INTERRUPT_GATE  // 0x8E
#define IDT_INT_USER IDT_ENTRY_PRES(1) | IDT_ENTRY_PRIV(3) | IDT_ENTRY_TYPE_32BIT_INTERRUPT_GATE  // 0x8E


typedef struct {
	uint16_t base_low;
	uint16_t selector; // Code segment selector
	uint8_t zero;
	uint8_t flags;
	uint16_t base_high;
} __attribute__ ((packed)) idt_entry_t;

typedef struct {
	uint16_t size;
	uint32_t offset;
} __attribute__ ((packed)) idt_pointer_t;

void idt_init();
void idt_set_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);


#endif
