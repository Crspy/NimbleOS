#include <kernel/multiboot.h>
#include <stddef.h>
#include <stdio.h>

void dump_multiboot_info(multiboot_info_t *mbi)
{
    /* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

    printf("=======Multiboot Header=======\n");
    if (mbi->flags & MULTIBOOT_INFO_BOOTDEV)
    {
        printf("Boot device infos:\n\tboot_device value:0x%x\n\tdrive number: %d\tpart1: %d\t part2: %d\t part3: %d\n", mbi->boot_device.raw_val, mbi->boot_device.drive_number,
               mbi->boot_device.part1, mbi->boot_device.part2, mbi->boot_device.part3);
    }

    /* Print out the flags. */
    printf("flags = 0x%X\n", (unsigned)mbi->flags);

    /* Are mem_* valid? */
    if (mbi->flags & MULTIBOOT_INFO_MEMORY)
        printf("mem_lower = %uKB, mem_upper = %uKB\n",
               (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

    /* Is boot_device valid? */
    if (mbi->flags & MULTIBOOT_INFO_BOOTDEV)
        printf("boot_device = 0x%x\n", mbi->boot_device);

    /* Is the command line passed? */
    if (mbi->flags & MULTIBOOT_INFO_CMDLINE)
        printf("cmdline = %s\n", (char *)mbi->cmdline);

    /* Are mods_* valid? */
    if (mbi->flags & MULTIBOOT_INFO_MODS)
    {
        multiboot_module_t *mod;
        size_t i;
        printf("mods_count = %d, mods_addr = 0x%x\n",
               (int)mbi->mods_count, (int)mbi->mods_addr);
        for (i = 0, mod = (multiboot_module_t *)mbi->mods_addr;
             i < mbi->mods_count;
             i++, mod++)
            printf(" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
                   (unsigned)mod->mod_start,
                   (unsigned)mod->mod_end,
                   (char *)mod->cmdline);
    }

    /* Bits 4 and 5 are mutually exclusive! */
    if ((mbi->flags & MULTIBOOT_INFO_AOUT_SYMS) && (mbi->flags & MULTIBOOT_INFO_ELF_SHDR))
    {
        printf("Both bits 4 and 5 are set.\n");
        return;
    }

    /* Is the symbol table of a.out valid? */
    if (mbi->flags & MULTIBOOT_INFO_AOUT_SYMS)
    {
        multiboot_aout_symbol_table_t *multiboot_aout_sym = &(mbi->u.aout_sym);

        printf("multiboot_aout_symbol_table: tabsize = 0x%0x, "
               "strsize = 0x%x, addr = 0x%x\n",
               (unsigned)multiboot_aout_sym->tabsize,
               (unsigned)multiboot_aout_sym->strsize,
               (unsigned)multiboot_aout_sym->addr);
    }

    /* Is the section header table of ELF valid? */
    if (mbi->flags & MULTIBOOT_INFO_ELF_SHDR)
    {
        multiboot_elf_section_header_table_t *multiboot_elf_sec = &(mbi->u.elf_sec);

        printf("multiboot_elf_sec: num = %u, size = 0x%x,"
               " addr = 0x%x, shndx = 0x%x\n",
               (unsigned)multiboot_elf_sec->num, (unsigned)multiboot_elf_sec->size,
               (unsigned)multiboot_elf_sec->addr, (unsigned)multiboot_elf_sec->shndx);
    }

    /* Are mmap_* valid? */
    if (mbi->flags & MULTIBOOT_INFO_MEM_MAP)
    {
        multiboot_memory_map_t *mmap;

        printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
               (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
        for (mmap = (multiboot_memory_map_t *)mbi->mmap_addr;
             (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
             mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
            printf(" size = 0x%x, base_addr = 0x%x%08x,"
                   " length = 0x%x%08x, type = 0x%x\n",
                   (unsigned)mmap->size,
                   (unsigned)(mmap->addr >> 32),
                   (unsigned)(mmap->addr & 0xffffffff),
                   (unsigned)(mmap->len >> 32),
                   (unsigned)(mmap->len & 0xffffffff),
                   (unsigned)mmap->type);
    }

    if (mbi->flags & MULTIBOOT_INFO_DRIVE_INFO)
    {
        printf("Drives: address=%X length=%d\n", mbi->drives_addr, mbi->drives_length);
    }

    if (mbi->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME)
    {
        printf("Boot loader name: %s\n", (char *)mbi->boot_loader_name);
    }

    if (mbi->flags & MULTIBOOT_INFO_APM_TABLE)
    {
        printf("APM: address=%X\n", mbi->apm_table);
    }

    if (mbi->flags & MULTIBOOT_INFO_VBE_INFO)
    {
        printf("VBE: not implemented\n");
    }

    // /* Draw diagonal blue line. */
    // if (mbi->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO)
    // {
    // 	multiboot_uint32_t color;
    // 	unsigned i;
    // 	void *fb = (void *)(unsigned long)mbi->framebuffer_addr;
    // 	printf("framebuffer_type: %d\n",mbi->framebuffer_type);
    // 	printf("framebuffer_addr: %x\n",mbi->framebuffer_addr);
    // 	switch (mbi->framebuffer_type)
    // 	{
    // 	case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
    // 	{
    // 		unsigned best_distance, distance;
    // 		struct multiboot_color *palette;

    // 		palette = (struct multiboot_color *)mbi->framebuffer_palette_addr;

    // 		color = 0;
    // 		best_distance = 4 * 256 * 256;

    // 		for (i = 0; i < mbi->framebuffer_palette_num_colors; i++)
    // 		{
    // 			distance = (0xff - palette[i].blue) * (0xff - palette[i].blue) + palette[i].red * palette[i].red + palette[i].green * palette[i].green;
    // 			if (distance < best_distance)
    // 			{
    // 				color = i;
    // 				best_distance = distance;
    // 			}
    // 		}
    // 	}
    // 	break;

    // 	case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
    // 		color = ((1 << mbi->framebuffer_blue_mask_size) - 1)
    // 				<< mbi->framebuffer_blue_field_position;
    // 		break;

    // 	case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
    // 		color = '\\' | 0x0100;
    // 		break;

    // 	default:
    // 		color = 0xffffffff;
    // 		break;
    // 	}
    // 	for (i = 0; i < mbi->framebuffer_width && i < mbi->framebuffer_height; i++)
    // 	{
    // 		switch (mbi->framebuffer_bpp)
    // 		{
    // 		case 8:
    // 		{
    // 			multiboot_uint8_t *pixel = fb + mbi->framebuffer_pitch * i + i;
    // 			*pixel = color;
    // 		}
    // 		break;
    // 		case 15:
    // 		case 16:
    // 		{
    // 			multiboot_uint16_t *pixel = fb + mbi->framebuffer_pitch * i + 2 * i;
    // 			*pixel = color;
    // 		}
    // 		break;
    // 		case 24:
    // 		{
    // 			multiboot_uint32_t *pixel = fb + mbi->framebuffer_pitch * i + 3 * i;
    // 			*pixel = (color & 0xffffff) | (*pixel & 0xff000000);
    // 		}
    // 		break;

    // 		case 32:
    // 		{
    // 			multiboot_uint32_t *pixel = fb + mbi->framebuffer_pitch * i + 4 * i;
    // 			*pixel = color;
    // 		}
    // 		break;
    // 		}
    // 	}
    // }
}