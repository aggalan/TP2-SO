#include "memoryManager.h"
#include "videoDriver.h"

typedef struct memory_manager {
    void * start;
    unint64_t size;
    uint32_t qty_blocks;
    uint32_t used_blocks;
    uint8_t * bitmap;
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
        mm.bitmap[i] =(uint8_t)FREE;
    }
}

void mm_init(uint64_t mem_size, void * mem_start) {

    if (mem_size > HEAP_SIZE) {
        drawWordColor("NOT ENOUGH MEMORY FOR HEAP INITIALIZATION", WHITE, RED);
        return;
    }
    mm.start = mem_start;
    mm.size = mem_size;
    mm.qty_blocks = mem_size/BLOCK_SIZE;
    mm.used_blocks = 0;
    mm.bitmap = (uint8_t *)mem_start;
    allocate_heap();
    mm.current = 0;
}


uint8_t find_contiguous_mem(uint8_t req_blocks) {
    uint8_t cont_blocks = 0;
    uint8_t start = mm.current;
    uint8_t index = mm.current;

    while (cont_blocks < req_blocks) {
        if (mm.bitmap[mm.current] == FREE) {
            cont_blocks++;
        } else {
            cont_blocks = 0;
            index = mm.current;
        }

        mm.current = (mm.current + 1) % mm.qty_blocks;

        if (mm.current == start) {
            return -1;
        }
    }

    return index;

}

void allocate_mem(uint8_t index, uint32_t size) {
    mm.bitmap[index] = (uint8_t)START;
    for (uint8_t i = 1; i < size; i++) {
        mm.bitmap[index + i] = (uint8_t)ALLOCATED;
    }
}

void * mm_malloc(uint32_t size) {
    uint8_t required_blocks = (size/BLOCK_SIZE) + 1

    int position = find_contiguous_mem(required_blocks);

    if (position == -1) {
        drawWordColor("NOT ENOUGH MEMORY FOR ALLOCATION", WHITE, RED);
        return NULL;
    }

    allocate_mem(position, required_blocks);

    return mm.start + position*BLOCK_SIZE;

}


void mm_free(void * ptr) {
    uint8_t index = (ptr - mm.start) / BLOCK_SIZE;
    if ((ptr - mm.start) % BLOCK_SIZE != 0 || mm.bitmap[index] != (uint8_t)START) {
        drawWordColor("INVALID MEMORY ADDRESS",WHITE,RED);
        return;
    }

    do {
        mm.bitmap[index++] = FREE;
    }while (mm.bitmap[index] == (uint8_t)ALLOCATED);

}




