#ifndef KHEAP_H
#define KHEAP_H



void* kmalloc(uint32_t size);
void* kamalloc(uint32_t size, uint32_t align);

#endif /* KHEAP */