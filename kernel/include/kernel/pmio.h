#ifndef PMIO_H
#define PMIO_H

#include <stdint.h>

unsigned char inportb(uint16_t port);
void outportb(uint16_t port, uint8_t data);

uint16_t inports(uint16_t port);
void outports(uint16_t port, uint16_t data);

uint32_t inportl(uint16_t port);
void outportl(uint16_t port, uint32_t data);

void inportsm(uint16_t port, unsigned char * data, uint32_t size);
void outportsm(uint16_t port, unsigned char * data, uint32_t size);

// Wait a very small amount of time (1 to 4 microseconds, generally). 
// Useful for implementing a small delay for PIC remapping on old hardware or generally as a simple but imprecise wait.
// You can do an IO operation on any unused port: the Linux kernel by default uses port 0x80, 
// which is often used during POST to log information on the motherboard's hex display but almost always unused after boot.
void io_wait(void);

#endif /* PMIO_H */
