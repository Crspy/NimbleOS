#include <stdio.h>

#include <kernel/keyboard.h>
#include <kernel/irq.h>
#include <kernel/pmio.h>


uint8_t keyboard_buffer[512];
uint32_t buf_length;

char sc_to_char[] = {
	0, 0,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[', ']', '\r', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
	';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', 0, '*', 0, ' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
	0, 0, 0, 0, 0,0,0,0
};

key_state_t keys_state[256];

void keyboard_init(void) {
	irq_register_handler(IRQ1, &keyboard_handler);
}

void keyboard_handler(registers_t*) {
	key_event_t key_event;
	uint8_t scancode = inportb(KBD_DATA);
	uint8_t sc = scancode & ~(KBD_KEY_RELEASED); // extract key code without key_released bit

	if (scancode & KBD_KEY_RELEASED) {
		keys_state[sc] = KEY_STATE_RELEASED;
		key_event.pressed = false;
	}
	else {
		keys_state[sc] = KEY_STATE_PRESSED;
		key_event.pressed = true;
	}

	key_event.key = sc;
	key_event.alt = keys_state[KEY_L_ALT];
	key_event.alt_gr = 0;
	key_event.shift = keys_state[KEY_L_SHIFT] || keys_state[KEY_R_SHIFT];
	key_event.super = keys_state[KEY_SUPER];
	key_event.control = keys_state[KEY_L_CTRL];
	key_event.scroll_lock = keys_state[KEY_SCROLLLOCK];
	key_event.caps_lock = keys_state[KEY_CAPSLOCK];
	key_event.num_lock = keys_state[KEY_NUMLOCK];

	if (sc_to_char[sc]) {
		printf("%c%s%s%s%s", sc_to_char[sc], key_event.alt ? " Alt " : "",
			key_event.shift ? " Shift " : "",
			key_event.control ? " Control " : "",
			key_event.super ? " Super " : "");
	}
}
