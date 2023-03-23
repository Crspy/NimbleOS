#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <kernel/irq.h>
#include <kernel/cpu.h>
#include <kernel/idt.h>
#include <kernel/pmio.h>

static handler_t irq_handlers[16] = { NULL };

/* This file is responsible for handling hardware interrupts, IRQs.
 */

/* Points IDT entries 32 to 47 to our IRQ handlers.
 * Those are defined in `irq.S`, and are responsible for calling `irq_handler`
 * below.
 */
void irq_init(void)
{
	irq_remap(IRQ0,IRQ8);

	idt_set_entry(IRQ0, (uint32_t)irq0, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ1, (uint32_t)irq1, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ2, (uint32_t)irq2, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ3, (uint32_t)irq3, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ4, (uint32_t)irq4, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ5, (uint32_t)irq5, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ6, (uint32_t)irq6, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ7, (uint32_t)irq7, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ8, (uint32_t)irq8, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ9, (uint32_t)irq9, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ10, (uint32_t)irq10, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ11, (uint32_t)irq11, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ12, (uint32_t)irq12, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ13, (uint32_t)irq13, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ14, (uint32_t)irq14, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(IRQ15, (uint32_t)irq15, GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);

	STI();
}

/* Calls the handler associated with calling IRQ, if any.
 */
void irq_handler(registers_t *regs)
{
	uint32_t irq = regs->int_no;

	// Handle spurious interrupts
	// Only for IRQ7 and IRQ15 when you're using the PIC/s.
	// For IRQ7, check the master PIC's ISR to see if it was a real IRQ7 or not. If it wasn't, don't send an EOI or anything.
	// For IRQ15, check the slave PIC's ISR to see if it was a real IRQ15 or not. If it wasn't, send an EOI to the master PIC but not to the slave PIC.
	if (irq == IRQ7 || irq == IRQ15) {
		uint16_t active_irq_mask = irq_get_isr();
		uint8_t current_irq_mask = 1 << (irq - IRQ0);
		if (!(active_irq_mask & current_irq_mask)) {
			if (irq == IRQ15) {
				outportb(PIC1_CMD, PIC_EOI); // send EOI to master PIC only
			}

			STI();
			return;
		}
	}

	irq_send_eoi(irq);

	handler_t handler = irq_handlers[irq - IRQ0];

	if (handler) {
		handler(regs);
	}
	else {
		printf("[IRQ] Unhandled IRQ%d\n", irq - IRQ0);
	}

	STI(); // re-enable interrupts
}

void irq_register_handler(uint8_t irq, handler_t handler) {
	assert(irq >= IRQ0 && irq <= IRQ15);

	if (!irq_handlers[irq - IRQ0]) {
		irq_handlers[irq - IRQ0] = handler;
	}
	else {
		printf("IRQ %d already registered\n", irq);
	}
}


void irq_remap(uint8_t offset1, uint8_t offset2)
{
#define ICW1_ICW4 0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE 0x02	/* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08		/* Level triggered (edge) mode */
#define ICW1_INIT 0x10		/* Initialization - required! */

#define ICW4_8086 0x01		 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02		 /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08	 /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10		 /* Special fully nested (not) */

	unsigned char a1, a2;

	a1 = inportb(PIC1_DATA); // save masks
	a2 = inportb(PIC2_DATA);

	outportb(PIC1_CMD, ICW1_INIT | ICW1_ICW4); // starts the initialization sequence (in cascade mode)
	io_wait();
	outportb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outportb(PIC1_DATA, offset1); // ICW2: Master PIC vector offset
	io_wait();
	outportb(PIC2_DATA, offset2); // ICW2: Slave PIC vector offset
	io_wait();
	outportb(PIC1_DATA, 4); // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outportb(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	outportb(PIC1_DATA, ICW4_8086);
	io_wait();
	outportb(PIC2_DATA, ICW4_8086);
	io_wait();

	outportb(PIC1_DATA, a1); // restore saved masks.
	outportb(PIC2_DATA, a2);
}

uint8_t irq_getmask(uint16_t pic)
{
	return inportb(pic);
}

void irq_setmask(uint16_t pic, uint8_t mask)
{
	outportb(pic, mask);
}

// Sets an IRQ number to hide
void irq_mask(uint8_t irq)
{
	uint16_t pic = 0;

	if (irq < 8)
	{
		pic = PIC1_DATA;
	}
	else
	{
		pic = PIC2_DATA;
		irq -= 8; // from range 8-15 to 0-7
	}

	uint8_t mask = irq_getmask(pic);
	mask |= (1 << irq);
	irq_setmask(pic, mask);
}

void irq_unmask(uint8_t irq)
{
	uint16_t pic = 0;

	if (irq < 8)
	{
		pic = PIC1_DATA;
	}
	else
	{
		pic = PIC2_DATA;
		irq -= 8;
	}

	uint8_t mask = irq_getmask(pic);
	mask &= ~(1 << mask);
	irq_setmask(pic, mask);
}
