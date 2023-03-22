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

#define IDT_ENTRY_FLAGS IDT_ENTRY_PRES(1) | IDT_ENTRY_PRIV(0) | IDT_ENTRY_TYPE_32BIT_INTERRUPT_GATE  // 0x8E


typedef struct {
	uint16_t base_low;
	uint16_t selector;
	uint8_t zero;
	uint8_t flags;
	uint16_t base_high;
} __attribute__ ((packed)) idt_entry_t;

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed)) idt_entry_ptr_t;

void idt_init();
void idt_set_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
extern void idt_load(uintptr_t idt_ptr);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr48();

#endif
