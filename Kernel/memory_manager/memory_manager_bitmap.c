#ifdef BITMAP
#include "./include/memory_manager.h"
#include "../Drivers/include/video_driver.h"
#include <stdio.h>
#include "scheduler.h"

typedef struct memory_manager {
    void * start;
    uint64_t size;
    uint64_t bitmap_size;
    uint32_t qty_blocks;
    uint32_t used_blocks;
    uint32_t * bitmap;
    uint32_t current;
} memory_manager;


memory_manager mm;

//static void full_memory_status();


void allocate_heap() {
    for (uint32_t i = 0; i < mm.qty_blocks; i++) {
        mm.bitmap[i] = FREE;
    }
}

void mm_init(void * mem_start, uint64_t mem_size) {

    uint64_t total_size = mem_size;

    if (mem_size > HEAP_SIZE) {
        drawWord1("NOT ENOUGH MEMORY FOR HEAP INITIALIZATION");
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
    mm.start = mem_start +  mm.bitmap_size * BLOCK_SIZE * sizeof(uint32_t);
    mm.used_blocks = 0;
    mm.bitmap = mem_start;
    mm.current = 0;

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
            if(cont_blocks == req_blocks)
            {
                return (void *)(mm.start + (*index) * BLOCK_SIZE);
            }
            return NULL; 
        }
    }

    return (void *)(mm.start + (*index) * BLOCK_SIZE);
}

void allocate_mem(uint32_t index, uint32_t blocks) {
    mm.bitmap[index] = START;
    for (uint32_t i = 1; i < blocks; i++) {
        mm.bitmap[index + i] = ALLOCATED;
    }
}

void * mm_malloc(uint32_t size) {
    uint32_t required_blocks = size/BLOCK_SIZE;
    uint32_t index = 0;

    if (required_blocks > mm.qty_blocks - mm.used_blocks) {
        drawWord1("NOT ENOUGH MEMORY FOR ALLOCATION");
        return NULL;
    }

    if (size % BLOCK_SIZE != 0) {
        required_blocks++;
    }

    void *ptr = find_contiguous_mem(required_blocks, &index);

    if (ptr == NULL) {  
        drawWord1("NOT ENOUGH MEMORY FOR ALLOCATION");
        return NULL;
    }

    allocate_mem(index, required_blocks);

    mm.used_blocks += required_blocks;
        
    return ptr;

}


void mm_free(void * ptr) {
    uint32_t index = (ptr - mm.start) / BLOCK_SIZE;

    if (index > mm.qty_blocks || mm.bitmap[index] != START) {
        drawWord1("INVALID MEMORY ADDRESS TO FREE");
        return;
    }

    do {
        mm.bitmap[index++] = FREE;
        mm.used_blocks--;
    }while (index < mm.qty_blocks && mm.bitmap[index] == ALLOCATED);

}

void mm_status() {
    drawWord1("Total Memory: ");
    drawNumber(mm.qty_blocks*BLOCK_SIZE);
    newLine();
    drawWord1("Total Memory In Use: ");
    drawNumber(mm.used_blocks*BLOCK_SIZE);
    newLine();
    drawWord1("Total Free: ");
    drawNumber((mm.qty_blocks - mm.used_blocks)*BLOCK_SIZE);
    newLine();
    //full_memory_status();


}


// void full_memory_status(){
//     uint64_t total = 0;
//     uint64_t total2 = 0;

//     for (uint32_t i = 0; i < mm.qty_blocks; i++)
//     {

//          if (mm.bitmap[i] != FREE && mm.bitmap[i] != ALLOCATED && mm.bitmap[i] != START)
//         {
//             total++;
//         }
//         if (mm.bitmap[i] == FREE)
//         {
//             total2++;
//         }
//     }
//     drawWord1("total memory free : ");
//     drawNumber(total2);
//     newLine();
//     drawWord1("total memory with garbage : ");
//     drawNumber(total);
//     newLine();
//     drawWord1("en pos 0 hay: ");
//     drawNumber(mm.bitmap[0]);
//     newLine();
//     print_processes();
// }

#endif