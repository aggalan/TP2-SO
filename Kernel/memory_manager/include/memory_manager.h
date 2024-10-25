#ifndef MEMORY_MANAGER_BITMAP_H
#define MEMORY_MANAGER_BITMAP_H

#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE       0x10000000  // 256MB heap
#define BLOCK_SIZE      32        //  bytes per block


typedef struct memory_manager {
    void * start;
    uint32_t blocks;
    uint32_t used;
    uint32_t * bitmap;
    size_t current;
} memory_manager;

typedef struct memory_manager * memory_manager_ADT;

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