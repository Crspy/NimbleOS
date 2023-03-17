#include <kernel/timer.h>
#include <kernel/irq.h>
#include <kernel/pmio.h>
#include <stdio.h>

volatile uint32_t current_tick = 0;

void timer_init(uint32_t desired_freq_hz) {
	irq_register_handler(IRQ0, timer_callback);

	uint32_t divisor = TIMER_QUOTIENT / desired_freq_hz;

	outportb(PIT_CMD, PIT_MODE);
	outportb(PIT_0, divisor & 0xFF);
	outportb(PIT_0, (divisor >> 8) & 0xFF);
}

void timer_callback(registers_t*) {
	current_tick++;
}

uint32_t timer_get_tick() {
	return current_tick;
}
