#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include <kernel/isr.h>




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
    printf("edi=0x%x, esi=0x%x, ebp=0x%x, esp=0x%x\n", reg->edi, reg->esi, reg->ebp, reg->esp);
    printf("eip=0x%x, cs=0x%x, ss=0x%x, eflags=0x%x, useresp=0x%x\n", reg->eip, reg->ss, reg->eflags, reg->useresp);
}

static handler_t isr_handlers[ISR_COUNT] = { NULL };

void isr_handler(registers_t* regs) {
    assert(regs->int_no < ISR_COUNT);

    if (isr_handlers[regs->int_no]) {
        handler_t handler = isr_handlers[regs->int_no];
        handler(regs);
    }
    else {
        printf("Unhandled hardware exception %d: %s\n", regs->int_no,
            regs->int_no < ISR_EXCEPTION_COUNT ? exception_msgs[regs->int_no] : "Unknown");
        print_registers(regs);
        abort();
    }
}

void isr_register_handler(uint32_t num, handler_t handler) {
	assert(num < ISR_COUNT);

    if (isr_handlers[num]) {
        printf("Exception handler %d (%s) already registered\n", num,
            num ? exception_msgs[num] : "Unknown");
    }
    else {
        isr_handlers[num] = handler;
    }
}
