#include "memoryManager.h"
#include "videoDriver.h"
#include <stdio.h>

typedef struct memory_manager {
    void * start;
    uint64_t size;
    uint64_t bitmap_size;
    uint32_t qty_blocks;
    uint32_t used_blocks;
    uint32_t * bitmap;
    uint32_t current;
} memory_manager;

typedef enum {
    FREE = 0,
    ALLOCATED = 1,
    START = 2
} block_state;

memory_manager mm;


void allocate_heap() {
    for (uint32_t i = 0; i < mm.qty_blocks; i++) {
        mm.bitmap[i] =(uint32_t)FREE;
    }
}

void mm_init(void * mem_start, uint64_t mem_size) {

    uint64_t total_size = mem_size;

    if (mem_size > HEAP_SIZE) {
        drawWordColor("NOT ENOUGH MEMORY FOR HEAP INITIALIZATION", WHITE, RED);
        return;
    }

    mm.qty_blocks = mem_size/BLOCK_SIZE;

    if(mem_size % BLOCK_SIZE != 0) {
        mm.qty_blocks++;
        total_size = mm.qty_blocks * BLOCK_SIZE;
    }

    mm.bitmap_size = mm.qty_blocks/BLOCK_SIZE;

    if(mm.qty_blocks % BLOCK_SIZE != 0) {
        mm.bitmap_size++;
    }

    total_size += mm.bitmap_size * BLOCK_SIZE;

    mm.size = total_size;
    mm.start = mem_start +  mm.bitmap_size * BLOCK_SIZE;
    mm.used_blocks = 0;
    mm.bitmap = mem_start;
    mm.current = 0;

    // printf("Heap initialized at %p\n", mm.start);

    allocate_heap();
}


void * find_contiguous_mem(uint32_t req_blocks, uint32_t * index) {
    uint32_t cont_blocks = 0;
    uint32_t start = mm.current;
    *index = mm.current;

    while (cont_blocks < req_blocks) {

        if(mm.current == 0 && cont_blocks > 0) {
            cont_blocks = 0;
        }

        if (mm.bitmap[mm.current] == FREE) {
            if (cont_blocks == 0) {
                *index = mm.current;
            }
            cont_blocks++;
         } else {
            cont_blocks = 0;
        }

        mm.current = (mm.current + 1) % mm.qty_blocks;

        if (mm.current == start) {
            return NULL; 
        }
    }

    return (void *)(mm.start + (*index) * BLOCK_SIZE);
}

void allocate_mem(uint32_t index, uint32_t blocks) {
    mm.bitmap[index] = (uint32_t)START;
    for (uint32_t i = 1; i < blocks; i++) {
        mm.bitmap[index + i] = (uint32_t)ALLOCATED;
    }
}

void * mm_malloc(uint32_t size) {
    uint32_t required_blocks = size/BLOCK_SIZE;
    uint32_t index = 0;

    if (required_blocks > mm.qty_blocks - mm.used_blocks) {
        // for (int i = 0; i < 10000000; i++) {
        //     printf("NOT ENOUGH MEMORY FOR ALLOCATION1 required blocks: %d, used blocks: %d, qty blocks: %d\n", required_blocks, mm.used_blocks, mm.qty_blocks);
        // }
        drawWordColor("NOT ENOUGH MEMORY FOR ALLOCATION", WHITE, RED);
        return NULL;
    }

    if (size % BLOCK_SIZE != 0) {
        required_blocks++;
    }

    void *ptr = find_contiguous_mem(required_blocks, &index);

    if (ptr == NULL) {  
        // for (int i = 0; i < 10000000; i++) {
        //     printf("NOT ENOUGH MEMORY FOR ALLOCATION2");
        // }
        drawWordColor("NOT ENOUGH MEMORY FOR ALLOCATION", WHITE, RED);
        return NULL;
    }

    // printf("index : %d\n", index);

    // printf("bitmap: %d\n", mm.bitmap[position]);

    allocate_mem(index, required_blocks);

    // printf("Memory allocated at %p for %d blocks for %d size\n", mm.start + index * BLOCK_SIZE, required_blocks, size);

    mm.used_blocks += required_blocks;
        
    return ptr;

}


void mm_free(void * ptr) {
    uint32_t index = (ptr - mm.start) / BLOCK_SIZE;
    if (index > mm.qty_blocks || mm.bitmap[index] != (uint32_t)START) {

        // for (int i = 0; i < 10000000; i++) {
        //     printf("INVALID MEMORY ADDRESS TO FREE");
        // }

        drawWordColor("INVALID MEMORY ADDRESS TO FREE", WHITE, RED);
        return;
    }

    do {
        mm.bitmap[index++] = FREE;
        mm.used_blocks--;
    }while (index < mm.qty_blocks && mm.bitmap[index] == (uint32_t)ALLOCATED);

}

void mm_status() {
    drawWord("Total Memory: ");
    drawNumber(mm.size - mm.bitmap_size*BLOCK_SIZE);
    newline();
    drawWord("Total Memory In Use: ");
    drawNumber(mm.used_blocks*BLOCK_SIZE);
    newline();
    drawWord("Total Free: ");
    drawNumber(mm.size - mm.bitmap_size*BLOCK_SIZE - mm.used_blocks*BLOCK_SIZE);
    newline();
}

