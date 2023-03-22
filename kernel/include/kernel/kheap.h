#ifndef KHEAP_H
#define KHEAP_H

#include <kernel/paging.h>

void* kmalloc(uint32_t size);

#endif /* KHEAP */