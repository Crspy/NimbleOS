#include <stdlib.h>
#include <ctype.h>

#include <kernel/ansi_interpreter.h>
#include <kernel/tty.h>

void ansi_init_context(ansi_interpreter_context* ctx) {
    ctx->state = NORMAL;
    ctx->current_arg = 0;
    ctx->current_index = 0;
    ctx->saved_row = 0;
    ctx->saved_col = 0;
}

// Returns != 0 if in an ANSI sequence
// ANSI escape sequence: \x1B[param;param2...end_char
bool ansi_interpret_char(ansi_interpreter_context* ctx, char c) {
    if (ctx->state == NORMAL) {
        if (c == 0x1B) { // Escape character
            ctx->state = BRACKET;
        }
        else {
            return false;
        }
    }
    else if (ctx->state == BRACKET) {
        if (c == '[') {
            ctx->state = PARAMS;
        }
        else {
            ctx->state = NORMAL;
            return false;
        }
    }
    else if (ctx->state == PARAMS) {
        if (c == ';') {
            ctx->buf[ctx->current_index] = '\0';
            ctx->args[ctx->current_arg++] = atoi(ctx->buf);
            ctx->current_index = 0;
        }
        else if (isdigit(c)) {
            if (ctx->current_index >= ANSI_BUFFER_SIZE) {
                ctx->current_arg = 0;
                ctx->current_index = 0;
                ctx->state = NORMAL;
            }
            else {
                ctx->buf[ctx->current_index++] = c;
            }
        }
        else if (isalpha(c)) {
            ctx->buf[ctx->current_index] = '\0';
            ctx->args[ctx->current_arg++] = atoi(ctx->buf);

            switch (c) {
            case 's': // Save cursor position
                ctx->saved_row = term_get_row();
                ctx->saved_col = term_get_column();
                ctx->state = NORMAL;
                break;
            case 'u': // Restore cursor position
                term_set_row(ctx->saved_row);
                term_set_column(ctx->saved_col);
                ctx->state = NORMAL;
                break;
            case 'K': // Erase until the end of line
                for (uint32_t x = term_get_column(); x < VGA_WIDTH; x++) {
                    term_putchar_at(' ', x, term_get_row());
                }
                ctx->state = NORMAL;
                break;
            case 'H': // Set cursor position
            case 'f':
                term_set_row(ctx->args[0]);
                term_set_column(ctx->args[1]);
                break;
            case 'A': // Cursor up
                term_set_row(term_get_row() - ctx->args[0]);
                break;
            case 'B': // Cursor down
                term_set_row(term_get_row() + ctx->args[0]);
                break;
            case 'C': // Cursor right
                term_set_column(term_get_column() + ctx->args[0]);
                break;
            case 'D': // Cursor left
                term_set_column(term_get_column() - ctx->args[0]);
                break;
            case 'J': // 2J: clear screen & reset cursor
                if (ctx->args[0] == 2) {
                    term_init();
                }
                break;
            }

            if (c == 'm') { // Set graphics mode
                for (uint32_t i = 0; i < ctx->current_arg; i++) {
                    switch (ctx->args[i]) {
                    case 0:
                        // regular text
                        term_set_blink(false); break;

                    case 1: break; // regular bold text
                    case 4: break; // regular underline text

                    case 5:
                        // regular blinking text
                        term_set_blink(true); break;
                    case 30:
                        term_set_fg_color(COLOR_BLACK); break;
                    case 31:
                        term_set_fg_color(COLOR_RED); break;
                    case 32:
                        term_set_fg_color(COLOR_GREEN); break;
                    case 33:
                        term_set_fg_color(COLOR_YELLOW); break;
                    case 34:
                        term_set_fg_color(COLOR_BLUE); break;
                    case 35:
                        term_set_fg_color(COLOR_MAGENTA); break;
                    case 36:
                        term_set_fg_color(COLOR_CYAN); break;
                    case 37:
                        term_set_fg_color(COLOR_WHITE); break;
                    case 40:
                        term_set_bg_color(COLOR_BLACK); break;
                    case 41:
                        term_set_bg_color(COLOR_RED); break;
                    case 42:
                        term_set_bg_color(COLOR_GREEN); break;
                    case 43: // should be "YELLOW" but in blinking mode we can't use anything above COLOR_LIGHT_GREY
                        term_set_bg_color(COLOR_BROWN); break;
                    case 44:
                        term_set_bg_color(COLOR_BLUE); break;
                    case 45:
                        term_set_bg_color(COLOR_MAGENTA); break;
                    case 46:
                        term_set_bg_color(COLOR_CYAN); break;
                    case 47: // should be "WHITE" but in blinking mode we can't use anything above COLOR_LIGHT_GREY
                        term_set_bg_color(COLOR_LIGHT_GREY); break;
                    }
                }
            }

            ctx->current_arg = 0;
            ctx->current_index = 0;
            ctx->state = NORMAL;
        }
    }

    return true;
}
