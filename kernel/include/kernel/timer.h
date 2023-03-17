#ifndef TIMER_H
#define TIMER_H

#include <kernel/isr.h>

void timer_init(uint32_t desired_freq_hz);
void timer_callback(registers_t* regs);
uint32_t timer_get_tick();

#define TIMER_IRQ IRQ0
#define TIMER_QUOTIENT 1193182

#define PIT_0 0x40
#define PIT_1 0x41
#define PIT_2 0x42
#define PIT_CMD 0x43

#define PIT_MODE 0x36    // square wave generator (operating mode)

#endif
