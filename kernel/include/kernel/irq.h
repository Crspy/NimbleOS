#ifndef IRQ_H
#define IRQ_H

#include <kernel/isr.h>


typedef void (*handler_t)(registers_t *);

void irq_init(void);
void irq_handler(registers_t *regs);
void irq_register_handler(uint8_t irq, handler_t handler);
/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void irq_remap(uint8_t offset1, uint8_t offset2);
uint8_t irq_getmask(uint16_t pic);
void irq_setmask(uint16_t pic, uint8_t mask);
void irq_mask(uint8_t irq);
void irq_unmask(uint8_t irq);


// exported from irq.S
void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_CMD PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_CMD PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20 /* End-of-interrupt command code */

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47


/*
* send an "End-of-interrupt" command.
* This is issued to the PIC chips at the end of an IRQ-based interrupt routine
* If the IRQ came from the Master PIC, it is sufficient to issue this command only to the Master PIC,
* however if the IRQ came from the Slave PIC, it is necessary to issue the command to both PIC chips.
*/
static inline void irq_send_eoi(uint32_t irq)
{
    // Received from slave
    if (irq > IRQ7)
    {
        outportb(PIC2_CMD, PIC_EOI);
    }

    // we have to send an EOI to the master regardless of the IRQ source
    outportb(PIC1_CMD, PIC_EOI);
}

#endif
