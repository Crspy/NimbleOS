#ifndef TTY_HEADER_H
#define TTY_HEADER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

typedef enum {
	COLOR_BLACK,
	COLOR_BLUE,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_RED,
	COLOR_MAGENTA,
	COLOR_BROWN,
	COLOR_LIGHT_GREY, // Max background color if blinking is enabled (since 7th bit is used for binking)
	COLOR_DARK_GREY,
	COLOR_LIGHT_BLUE,
	COLOR_LIGHT_GREEN,
	COLOR_LIGHT_CYAN,
	COLOR_LIGHT_RED,
	COLOR_LIGHT_MAGENTA,
	COLOR_YELLOW,
	COLOR_WHITE,
	COLOR_COUNT
} term_vga_color;

void term_init(void);
void term_putchar_at(char c, size_t x, size_t y);
void term_setcolor(term_vga_color fg, term_vga_color bg);
void term_set_blink(bool blink);
void term_change_bg_color(term_vga_color bg);
void term_scrolldown(void);
void term_putchar(char c);
void term_write(const char* data, size_t size);
void term_writestring(const char* data);

// Returns true if in an ANSI sequence
// ANSI escape sequence: \x1B[param;param2...end_char
bool term_interpret_ansi(char c);

#endif
