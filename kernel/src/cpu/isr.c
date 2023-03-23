#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/cpu.h>




static char* exception_msgs[ISR_EXCEPTION_COUNT] = {
    "Division By Zero",
    "Debugger",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bounds",
    "Invalid Opcode",
    "Coprocessor Not Available",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Invalid Task State Segment",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "Math Fault",
    "Alignement Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

static void print_registers(registers_t* reg)
{
    printf("REGISTERS:\n");
    printf("err_code=%d\n", reg->err_code);
    printf("eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x\n", reg->eax, reg->ebx, reg->ecx, reg->edx);
    printf("edi=0x%x, esi=0x%x, ebp=0x%x, kernel_esp=0x%x\n", reg->edi, reg->esi, reg->ebp, reg->kernel_esp);
    printf("eip=0x%x, cs=0x%x, ss=0x%x, eflags=0x%x, esp=0x%x\n", reg->eip, reg->ss, reg->eflags, reg->esp);
}

static handler_t isr_handlers[ISR_COUNT] = { NULL };

/* This file is responsible for handling software interrupts, ISRs.
 */

/* Points reserved IDT entries to our ISR handlers, along with the entry for the
 * syscall interrupt.
 * ISR handlers are defined in `isr.S` and all call `isr_handler`, defined here.
 */
void isr_init()
{
    idt_set_entry(0, (uint32_t)  isr0,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(1, (uint32_t)  isr1,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(2, (uint32_t)  isr2,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(3, (uint32_t)  isr3,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(4, (uint32_t)  isr4,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(5, (uint32_t)  isr5,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(6, (uint32_t)  isr6,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(7, (uint32_t)  isr7,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(8, (uint32_t)  isr8,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(9, (uint32_t)  isr9,   GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(10, (uint32_t) isr10,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(11, (uint32_t) isr11,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(12, (uint32_t) isr12,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(13, (uint32_t) isr13,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(14, (uint32_t) isr14,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(15, (uint32_t) isr15,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(16, (uint32_t) isr16,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(17, (uint32_t) isr17,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(18, (uint32_t) isr18,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(19, (uint32_t) isr19,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(20, (uint32_t) isr20,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(21, (uint32_t) isr21,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(22, (uint32_t) isr22,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(23, (uint32_t) isr23,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(24, (uint32_t) isr24,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(25, (uint32_t) isr25,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(26, (uint32_t) isr26,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(27, (uint32_t) isr27,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(28, (uint32_t) isr28,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(29, (uint32_t) isr29,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(30, (uint32_t) isr30,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);
	idt_set_entry(31, (uint32_t) isr31,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_KERNEL);

    // Syscall interrupt gate
	idt_set_entry(48, (uint32_t) isr48,  GDT_SELECTOR_KERNEL_CODE, IDT_INT_USER); // syscall
}

/* Calls the handler registered to a specific interrupt, if any.
 * This function is called from the real interrupt handlers set up in
 * `init_isr`, defined in `isr.S`.
 */
void isr_handler(registers_t* regs) {
    assert(regs->int_no < ISR_COUNT);

    if (isr_handlers[regs->int_no]) {
        
        handler_t handler = isr_handlers[regs->int_no];
        handler(regs);
    }
    else {
		printf("[ISR] Unhandled %s %d: %s\n",
		    regs->int_no < 32 ? "exception" : "interrupt", regs->int_no,
		    regs->int_no < 32 ? exception_msgs[regs->int_no] : "Unknown");
        print_registers(regs);
		abort();
    }
}

/* Registers a handler to be called when interrupt `num` fires.
 */
void isr_register_handler(uint32_t num, handler_t handler) {
	assert(num < ISR_COUNT);

    if (isr_handlers[num]) {
		printf("[ISR] Interrupt handler %d already registered\n", num,
		    exception_msgs[num]);
    }
    else {
        isr_handlers[num] = handler;
    }
}
