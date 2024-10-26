#ifdef BITMAP
#include "./include/memory_manager.h"
#include "../Drivers/include/video_driver.h"
#include <stdio.h>
#include "scheduler.h"

memory_manager_ADT mm;


void bitmap_init() {
    for (uint32_t i = 0; i < mm->blocks; i++) {
        mm->bitmap[i] = FREE;
    }
}

void mm_init(void * base, uint64_t size) {

    uint64_t total = size;
    uint32_t blocks = size / BLOCK_SIZE;

    if(size % BLOCK_SIZE != 0){
        blocks++;
        total = BLOCK_SIZE * blocks;
    }

    uint32_t b_size = blocks / BLOCK_SIZE;

    if(blocks % BLOCK_SIZE){
        b_size++;
    }

   total += b_size * BLOCK_SIZE;

   if (total > HEAP_SIZE) {
        drawWord1("NOT ENOUGH MEMORY FOR HEAP INITIALIZATION");
        return;
    }

    mm->blocks = blocks;
    mm->start = base +  b_size * BLOCK_SIZE * sizeof(uint32_t);
    mm->used= 0;
    mm->bitmap = base;
    mm->current = 0;

    bitmap_init();
}


void * mm_find(uint32_t required, uint32_t * index) {
    uint32_t free = 0;
    uint32_t start = mm->current;
    *index = mm->current;

    while (free < required) {

        if(mm->current == 0 && free > 0) {
            free = 0;
        }

        if (mm->bitmap[mm->current] == FREE) {
            if (free == 0) {
                *index = mm->current;
            }
            free++;
            
         } else {
            free = 0;
        }

        mm->current = (mm->current + 1) % mm->blocks;

        if (mm->current == start) {
            if(free == required)
            {
                return (void *)(mm->start + (*index) * BLOCK_SIZE);
            }
            return NULL; 
        }
    }

    return (void *)(mm->start + (*index) * BLOCK_SIZE);
}

void mm_fill(uint32_t index, uint32_t blocks) {
    mm->bitmap[index] = START;
    for (uint32_t i = 1; i < blocks; i++) {
        mm->bitmap[index + i] = ALLOCATED;
    }
}

void * mm_malloc(uint32_t size) {
    uint32_t required = size/BLOCK_SIZE;
    uint32_t index = 0;

    if (required > mm->blocks - mm->used) {
        drawWord1("NOT ENOUGH MEMORY FOR ALLOCATION");
        return NULL;
    }

    if (size % BLOCK_SIZE != 0) {
        required++;
    }

    void *ptr = mm_find(required, &index);

    if (ptr == NULL) {  
        drawWord1("NOT ENOUGH MEMORY FOR ALLOCATION");
        return NULL;
    }

    mm_fill(index, required);

    mm->used += required;
        
    return ptr;

}


void mm_free(void * ptr) {
    uint32_t index = (ptr - mm->start) / BLOCK_SIZE;

    if (index > mm->blocks || mm->bitmap[index] != START) {
        //drawWord1("INVALID MEMORY ADDRESS TO FREE");
        return;
    }

    do {
        mm->bitmap[index++] = FREE;
        mm->used--;
    }while (index < mm->blocks && mm->bitmap[index] == ALLOCATED);

}

void mm_status() {
    drawWord1("Running on Bitmap System");
    newLine();
    drawWord1("TOTAL: ");
    drawNumber(mm->blocks*BLOCK_SIZE);
    newLine();
    drawWord1("USED: ");
    drawNumber(mm->used*BLOCK_SIZE);
    newLine();
    drawWord1("FREE: ");
    drawNumber((mm->blocks - mm->used)*BLOCK_SIZE);
    newLine();
}

#endif