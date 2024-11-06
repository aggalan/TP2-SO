
#ifndef MEMORY_MANAGER_BITMAP_H
#define MEMORY_MANAGER_BITMAP_H

#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE 0x10000000 // 256MB heap
#define BLOCK_SIZE 32        //  bytes per block
#define HEAP_SIZE 0x10000000 // 256MB heap
#define BLOCK_SIZE 32        //  bytes per block


typedef struct memory_status_t
{
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
} memory_status_t;

typedef enum BlockStatus
{
    FREE,
    ALLOCATED,
    START
} BlockStatus;

void *mm_malloc(uint32_t size);
void mm_free(void *ptr);
void mm_init(void *mem_start, uint64_t mem_size);
void mm_status();

#endif