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
void term_putchar_at(char c, uint32_t x, uint32_t y);

void term_set_blink(bool blink);
void term_change_bg_color(term_vga_color bg);
void term_scrolldown(void);
void term_putchar(char c);
void term_write(const uint8_t* data, uint32_t size);
void term_write_string(const uint8_t* data);

// Getters and setters, used by the ANSI escape sequences interpreter
uint32_t term_get_row();
uint32_t term_get_column();
uint8_t term_get_color();
uint8_t term_get_fg_color();
uint8_t term_get_bg_color();
uint16_t* term_get_buffer();

void term_set_row(uint32_t row);
void term_set_column(uint32_t column);
void term_set_color(uint8_t color);
void term_set_fg_color(term_vga_color color);
void term_set_bg_color(term_vga_color color);
void term_set_buffer(uint16_t* buffer);

#endif
