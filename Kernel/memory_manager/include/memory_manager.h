#ifndef MEMORY_MANAGER_BITMAP_H
#define MEMORY_MANAGER_BITMAP_H

#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE       0x10000000  // 256MB heap
#define BLOCK_SIZE      32        //  bytes per block


int mm_init(void * base, uint64_t size);
void bitmap_init();
void * mm_malloc(size_t size);
void mm_fill(size_t required, size_t first);
void mm_free(void * ptr);
void mm_status();
uintptr_t mm_find(size_t required, size_t start, size_t end);


#endif