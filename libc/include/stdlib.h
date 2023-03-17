#ifndef STDLIB_H
#define STDLIB_H

#include <std_abs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort();

int atoi(const char* s);
long atol(const char* s);
long long atoll(const char* s);


// non-standard extensions
char* itoa(int value, char* str, int base);
char* utoa(unsigned int value, char* str,unsigned int base);

#ifdef __cplusplus
}
#endif

#endif
