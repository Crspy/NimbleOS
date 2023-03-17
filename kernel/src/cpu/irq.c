#include <kernel/irq.h>
#include <kernel/cpu.h>
#include <kernel/idt.h>
#include <kernel/pmio.h>
#include <stdint.h>
#include <string.h>

#include <stdio.h>

static handler_t irq_handlers[256];

void irq_init(void)
{
	irq_remap(32,40);

	memset(irq_handlers, 0, 256 * sizeof(handler_t));

	idt_set_entry(32, (uint32_t)irq0, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(33, (uint32_t)irq1, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(34, (uint32_t)irq2, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(35, (uint32_t)irq3, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(36, (uint32_t)irq4, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(37, (uint32_t)irq5, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(38, (uint32_t)irq6, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(39, (uint32_t)irq7, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(40, (uint32_t)irq8, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(41, (uint32_t)irq9, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(42, (uint32_t)irq10, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(43, (uint32_t)irq11, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(44, (uint32_t)irq12, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(45, (uint32_t)irq13, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(46, (uint32_t)irq14, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
	idt_set_entry(47, (uint32_t)irq15, GDT_SELECTOR_CODE0, IDT_ENTRY_FLAGS);
}

void irq_handler(registers_t *regs)
{
	CLI();

	if (irq_handlers[regs->int_no])
	{
		handler_t handler = irq_handlers[regs->int_no];
		handler(regs);
	}

	// Don't annoy the PIC with software interrupts
	if (regs->int_no >= IRQ0 && regs->int_no <= IRQ15)
	{
		irq_send_eoi(regs->int_no);

	}
	STI();
}

void irq_register_handler(uint8_t irq, handler_t handler)
{
	CLI();

	if (!irq_handlers[irq])
	{
		irq_handlers[irq] = handler;
	}
	else
	{
		printf("IRQ %d already registered\n", irq);
	}

	STI();
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
