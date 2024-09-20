#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#define HEAP_SIZE       0x10000000  // 256MB heap REVISAR
#define BLOCK_SIZE      64        // 64 bytes per block
#define NUM_BLOCKS      (HEAP_SIZE / BLOCK_SIZE) // Number of blocks in the heap
#define BITMAP_SIZE     NUM_BLOCKS // Size of the bitmap


void * mm_malloc(uint32_t size);
void mm_free(void * ptr);
void mm_init(uint64_t mem_size, void * mem_start);


#endif