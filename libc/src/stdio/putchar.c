#include <stdio.h>

#if defined(_KERNEL_)
#include <kernel/tty.h>
#endif

int putchar(int ic)
{
#if defined(_KERNEL_)
	char c = (char) ic;
	terminal_putchar(c);
#else
	// TODO: You need to implement a write system call.
#endif
	return ic;
}
