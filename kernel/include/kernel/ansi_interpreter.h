#ifndef ANSI_INTERPRETER_H
#define ANSI_INTERPRETER_H

#include <stdint.h>
#include <stdbool.h>

#define ANSI_BUFFER_SIZE 32

typedef enum {
	NORMAL, BRACKET, PARAMS
} ansi_interpreter_state;

typedef struct {
	ansi_interpreter_state state;    // State tracker
	char buf[ANSI_BUFFER_SIZE];      // Stores the string for 1 param
	uint32_t args[ANSI_BUFFER_SIZE]; // Parsed args
	uint32_t current_arg;            // Current number of params
	uint32_t current_index;          // Number of chars in the current param
	uint32_t saved_row;
	uint32_t saved_col;
} ansi_interpreter_context;

void ansi_init_context(ansi_interpreter_context* ctx);

// Returns true if in an ANSI sequence
// ANSI escape sequence: \x1B[param;param2...end_char
bool ansi_interpret_char(ansi_interpreter_context* ctx, char c);

#endif
