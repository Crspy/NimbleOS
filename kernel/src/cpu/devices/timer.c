#include <stdio.h>

#include <kernel/timer.h>
#include <kernel/irq.h>
#include <kernel/pmio.h>


volatile uint32_t current_tick = 0;
static handler_t callback;

void timer_init() {
	uint32_t divisor = TIMER_QUOTIENT / TIMER_FREQ;

	outportb(PIT_CMD, PIT_MODE);
	outportb(PIT_0, divisor & 0xFF);
	outportb(PIT_0, (divisor >> 8) & 0xFF);

	irq_register_handler(TIMER_IRQ, timer_callback);
}

void timer_callback(registers_t* regs) {
	current_tick++;
	if (callback) {
		callback(regs);
	}
}

uint32_t timer_get_tick() {
	return current_tick;
}

/* Returns the time since boot in seconds
 */
double timer_get_time() {
	return current_tick * (1.0 / TIMER_FREQ);
}

void timer_register_callback(handler_t handler) {
	if (callback) {
		printf("[TIMER] Callback already registered!\n");
		printf("[TIMER] Add support for multiple callbacks!\n");
	} else {
		callback = handler;
	}
}
