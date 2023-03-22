#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <kernel/isr.h>


typedef int(*sys_handler_t)(registers_t*);

void syscall_init();
void syscall_register_handler(uint32_t num, sys_handler_t handler);

#endif
