#ifndef CPU_HEADER_H
#define CPU_HEADER_H

#define GDT_SELECTOR_CODE0 0x08
#define GDT_SELECTOR_DATA0 0x10
#define GDT_SELECTOR_CODE3 0x18
#define GDT_SELECTOR_DATA3 0x20

#ifndef __ASSEMBLY__
// insert any code here that we don't want to be included in assembly files
#define CLI() asm volatile("cli")
#define STI() asm volatile("sti")
#define HLT() asm volatile("hlt")






#endif

#endif  /* CPU_HEADER_H */