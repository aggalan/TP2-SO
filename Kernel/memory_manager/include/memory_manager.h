#ifndef MEMORY_MANAGER_BITMAP_H
#define MEMORY_MANAGER_BITMAP_H

#include <stdint.h>

#define HEAP_SIZE       0x10000000  // 256MB heap
#define BLOCK_SIZE      32        //  bytes per block
#define NULL            ((void *)0)

typedef enum BlockStatus {
    FREE,
    ALLOCATED,
    START
}BlockStatus;


void * mm_malloc(uint32_t size);
void mm_free(void * ptr);
void mm_init(void * mem_start, uint64_t mem_size );
void mm_status();


#endif