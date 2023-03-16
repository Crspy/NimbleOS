#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int printf(const char *format, ...)
{
    char **arg = (char **)&format;
    int c;
    char buf[32];

    int written = 0;

    arg++;

    while ((c = *format++) != 0)
    {
        if (c != '%')
        {
            putchar(c);
            written++;
        }
        else
        {

            int pad0 = 0, pad_count = 0;

            c = *format++;
            if (c == '0')
            {
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9')
            {
                pad_count = c - '0';
                c = *format++;
            }

            char *p;

            switch (c)
            {
            case 'd':
                itoa(*((int *)arg++), buf, 10);
                break;
            case 'u':
                utoa(*((int *)arg++), buf, 10);
                break;
            case 'x':
            case 'X':
                utoa(*((int *)arg++), buf, 16);
                break;
            }

            switch (c)
            {
            case 'X':
                for (size_t i = 0; buf[i]; ++i)
                {
                    buf[i] = toupper(buf[i]);
                }
                /* fall through */
            case 'd':
            case 'u':
            case 'x':
            
                p = buf;
                goto string;
                break;

            case 's':
                p = *arg++;
                if (!p)
                    p = "(null)";

            string:
                // print padding
                {
                    char *p2;
                    for (p2 = p; *p2; p2++)
                        ;
                    for (; p2 < p + pad_count; p2++)
                    {
                        putchar(pad0 ? '0' : ' ');
                        written++;
                    }
                }

                while (*p)
                {
                    putchar(*p++);
                    written++;
                }
                break;

            default:
                putchar(*((int *)arg++));
                written++;
                break;
            }
        }
    }
    return written;
}