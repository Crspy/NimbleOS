
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/pmm.h>
#include <kernel/paging.h>

static uint32_t mem_size;
static uint32_t used_blocks;
static uint32_t max_blocks;

uint32_t* bitmap;

// Linker-provided symbol
extern uint32_t KERNEL_END;
extern uint32_t KERNEL_END_PHYS;

void pmm_init(multiboot_info_t* mbi) {
	mem_size = mbi->mem_lower + mbi->mem_upper;
	bitmap = &KERNEL_END;
	max_blocks = (mem_size * 1024) / PMM_BLOCK_SIZE;
	used_blocks = max_blocks;

	// we only need (max_blocks/8 bytes -> max_blocks bits) of memory to store bitmap states (1/0) which corresponds to (used/unused)
	memset(bitmap, 0xFF, max_blocks / 8);

	uint64_t available = 0;
	uint64_t unavailable = 0;

	multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)P2V(mbi->mmap_addr);

	while ((uintptr_t)mmap < (P2V(mbi->mmap_addr) + mbi->mmap_length)) {
		if (!mmap->len) {
			continue;
		}

		if (mmap->type == 1) {
			pmm_init_region((uintptr_t)mmap->addr, mmap->len);
			available += mmap->len;
		}
		else {
			unavailable += mmap->len;
		}

		// Casts needed to get around pointer increment magic
		mmap = (multiboot_memory_map_t*)((uintptr_t)mmap + mmap->size + sizeof(uintptr_t));
	}

	// Protect low memory, kernel and the PMM itself
	pmm_deinit_region((uintptr_t)0, (uint32_t)&KERNEL_END_PHYS + max_blocks / 8);

	printf("[PMM] Memory stats: available: \x1B[32m%dMB", available / (1024 * 1024));
	printf("\x1B[37m unavailable: \x1B[32m%dKB\x1B[37m\n", unavailable / 1024);


}

uint32_t pmm_get_map_size() {
	return max_blocks / 8;
}

void pmm_init_region(uintptr_t addr, uint32_t size) {
	// block number this region starts at
	uint32_t base_block = addr / PMM_BLOCK_SIZE;

	// number of blocks this region represents
	uint32_t num = size / PMM_BLOCK_SIZE;

	printf("[PMM] Initializing blocks %d to %d at addr %p\n", base_block, base_block + num, addr);

	while (num-- > 0) {
		mmap_unset(base_block++);
	}

}

void pmm_deinit_region(uintptr_t addr, uint32_t size) {
	uint32_t base_block = addr / PMM_BLOCK_SIZE;
	uint32_t num = size / PMM_BLOCK_SIZE;

	printf("[PMM] Retaining blocks %d to %d\n", base_block, base_block + num);

	while (num-- > 0) {
		mmap_set(base_block++);
	}
}

uintptr_t pmm_alloc_page() {
	if (max_blocks - used_blocks <= 0) {
		printf("[PMM] Kernel is out of physical memory!");
		abort();
	}

	uint32_t block = mmap_find_free();

	if (!block) {
		return 0;
	}

	mmap_set(block);

	return (uintptr_t)(block * PMM_BLOCK_SIZE);
}


uintptr_t pmm_alloc_aligned_large_page() {

	/* Strategy:
	 * Find an 8 MB section of memory. In there, we are guaranteed to find an
	 * address that is 4 MB-aligned. Return that, mark 4 MB as taken.
	 */
	 // TODO: generalize
	if (max_blocks - used_blocks < 2 * 1024) { // 4MB
		return 0;
	}

	uint32_t free_block = mmap_find_free_frame(2 * 1024);

	if (!free_block) {
		return 0;
	}

	uint32_t aligned_block = ((free_block / 1024) + 1) * 1024;

	for (int i = 0; i < 1024; i++) {
		mmap_set(aligned_block + i);
	}

	return (uintptr_t)(aligned_block * PMM_BLOCK_SIZE);
}

uintptr_t pmm_alloc_pages(uint32_t num) {
	if (max_blocks - used_blocks < num) {
		return 0;
	}

	uint32_t first_block = mmap_find_free_frame(num);

	if (!first_block) {
		return 0;
	}

	for (uint32_t i = 0; i < num; i++) {
		mmap_set(first_block + i);
	}

	return (uintptr_t)(first_block * PMM_BLOCK_SIZE);
}

void pmm_free_page(uintptr_t addr) {
	uint32_t block = addr / PMM_BLOCK_SIZE;
	mmap_unset(block);
}

void pmm_free_pages(uintptr_t addr, uint32_t num) {
	uint32_t first_block = addr / PMM_BLOCK_SIZE;

	for (uint32_t i = 0; i < num; i++) {
		mmap_unset(first_block + i);
	}
}

void mmap_set(uint32_t bit) {
	bitmap[bit / 32] |= (1 << (bit % 32));
	used_blocks++;
}

void mmap_unset(uint32_t bit) {
	bitmap[bit / 32] &= ~(1 << (bit % 32));
	used_blocks--;
}

uint32_t mmap_test(uint32_t bit) {
	return bitmap[bit / 32] & (1 << (bit % 32));
}

// Returns the first free bit
uint32_t mmap_find_free() {
	for (uint32_t i = 0; i < max_blocks / 32; i++) {
		if (bitmap[i] != 0xFFFFFFFF) {
			for (uint32_t j = 0; j < 32; j++) {
				if (!(bitmap[i] & (1 << j))) {
					return i * 32 + j;
				}
			}
		}
	}

	return 0;
}

// Returns the first block of frame_size bits
uint32_t mmap_find_free_frame(uint32_t frame_size) {
	uint32_t first = 0;
	uint32_t count = 0;

	for (uint32_t i = 0; i < max_blocks / 32; i++) {
		if (bitmap[i] != 0xFFFFFFFF) {
			for (uint32_t j = 0; j < 32; j++) {
				if (!(bitmap[i] & (1 << j))) {
					if (!first) {
						first = i * 32 + j;
					}

					count++;
				}
				else {
					first = 0;
					count = 0;
				}

				if (count == frame_size) {
					return first;
				}
			}
		}
		else {
			first = 0;
			count = 0;
		}
	}

	return 0;
}
