#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <kernel/pmio.h>
#include <kernel/tty.h>
#include <kernel/ansi_interpreter.h>

#define VGA_ENTRY(x, y) term_buffer[(y)*VGA_WIDTH + (x)]

static ansi_interpreter_context ctx;
static uint32_t term_row;
static uint32_t term_column;
static uint8_t term_color;
static uint16_t* term_buffer;

// Helper functions
static uint8_t term_make_color(term_vga_color fg, term_vga_color bg) {
	return fg | bg << 4;
}

// Entry format: [ BLINK BG BG BG FG FG FG FG | C C C C C C C C ]
//               [           8-bits           |      8-bits     ]
static uint16_t term_make_entry(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}



// Terminal handling functions
void term_init(void)
{
	// uint8_t status = inportb(0x3DA);
	// uint16_t port_addr = 0x3C0;
	// outportb(port_addr, 0x10);
	// uint8_t status2 = inportb(port_addr + 1);
	// status2 |= (1 << 3);
	// outportb(port_addr, status2);
	// status = inportb(0x3DA);
	// outportb(port_addr, 0x30);

	uint16_t entry = term_make_entry(' ', term_color);
	term_buffer = (uint16_t*)VGA_MEMORY;
	term_color = term_make_color(COLOR_WHITE, COLOR_BLACK);
	term_row = 0;
	term_column = 0;

	for (uint32_t x = 0; x < VGA_WIDTH; x++)
	{
		for (uint32_t y = 0; y < VGA_HEIGHT; y++)
		{
			VGA_ENTRY(x, y) = entry;
		}
	}
}

void term_setcolor(term_vga_color fg, term_vga_color bg)
{
	term_color = term_make_color(fg, bg);
}

void term_set_blink(bool blink)
{
#define VGA_ENTRY_BLINK_MASK (1 << 7)

	if (blink)
	{
		term_color |= VGA_ENTRY_BLINK_MASK;
	}
	else
	{
		term_color &= ~VGA_ENTRY_BLINK_MASK;
	}
}

void term_change_bg_color(term_vga_color bg)
{
	for (uint32_t x = 0; x < VGA_WIDTH; x++)
	{
		for (uint32_t y = 0; y < VGA_HEIGHT; y++)
		{
			uint16_t entry = VGA_ENTRY(x, y);
			char c = entry & 0xFF;
			term_vga_color fg = (entry & 0x0F00) >> 8;
			VGA_ENTRY(x, y) = term_make_entry(c, term_make_color(fg, bg));
		}
	}

	term_set_bg_color(bg);
}

void term_putchar_at(char c, uint32_t x, uint32_t y)
{
	if (y >= VGA_HEIGHT || x >= VGA_WIDTH)
	{
		return;
	}

	VGA_ENTRY(x, y) = term_make_entry(c, term_color);
}

void term_scrolldown(void)
{
	for (uint32_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (uint32_t x = 0; x < VGA_WIDTH; x++)
		{
			if (y < VGA_HEIGHT - 1)
			{
				VGA_ENTRY(x, y) = VGA_ENTRY(x, y + 1);
			}
			else
			{ // last line
				VGA_ENTRY(x, y) = term_make_entry(' ', term_color);
			}
		}
	}

	term_row--;
}

void term_putchar(char c)
{
	if (c == '\n' || c == '\r')
	{
		term_row += 1;
		term_column = 0;
	}
	else if (c == '\t')
	{
		for (int i = 0; i < 4; i++)
		{
			term_putchar_at(' ', term_column++, term_row);
		}
	}

	if (term_column >= VGA_WIDTH)
	{
		term_row += 1;
		term_column = 0;
	}

	if (term_row >= VGA_HEIGHT)
	{
		term_scrolldown();
	}

	if (ansi_interpret_char(&ctx, c) || !isprint(c))
		return;

	term_putchar_at(c, term_column++, term_row);
}

void term_write(const char* data, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		term_putchar(data[i]);
	}
}

void term_write_string(const char* data)
{
	term_write(data, strlen(data));
}

// Getters
uint32_t term_get_row() {
	return term_row;
}

uint32_t term_get_column() {
	return term_column;
}

uint8_t term_get_color() {
	return term_color;
}
uint8_t term_get_fg_color() {
	return term_color & 0x000F;
}

uint8_t term_get_bg_color() {
	return term_color >> 4;
}

uint16_t* term_get_buffer() {
	return term_buffer;
}

// Setters
void term_set_row(uint32_t row) {
	term_row = row;
}

void term_set_column(uint32_t column) {
	term_column = column;
}

void term_set_color(uint8_t color) {
	term_color = color;
}

void term_set_bg_color(term_vga_color color) {
	term_color = term_make_color(term_get_fg_color(), color);
}

void term_set_fg_color(term_vga_color color) {
	term_color = term_make_color(color, term_get_bg_color());
}

void term_set_buffer(uint16_t* buffer) {
	term_buffer = buffer;
}

