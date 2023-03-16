#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort();

char* itoa(int value, char* str, int base);
char* utoa(unsigned int value, char* str,unsigned int base);

int atoi(const char* s);

#ifdef __cplusplus
}
#endif

#endif
