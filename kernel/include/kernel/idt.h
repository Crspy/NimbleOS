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
void idt_load(uint32_t idt_ptr);

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

#endif
