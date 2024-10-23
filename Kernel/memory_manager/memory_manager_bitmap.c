#ifdef BITMAP
#include "./include/memory_manager.h"
#include "../Drivers/include/video_driver.h"
#include <stdio.h>
#include "scheduler.h"

typedef struct{
    void * start;
    uint32_t blocks;
    uint32_t used;
    uint32_t * bitmap;
    size_t current;
}mm;

typedef enum {
    FREE = 0,
    ALLOCATED = 1,
    START = 2
} state;

mm manager;

int mm_init(void * base, uint64_t size);
void bitmap_init();
void * mm_malloc(size_t size);
void mm_fill(size_t required, size_t first);
void mm_free(void * ptr);
void mm_status();
uintptr_t mm_find(size_t required, size_t start, size_t end);
int trash();



int mm_init(void * base, uint64_t size){
    size_t total = size;
    uint32_t blocks = size / BLOCK_SIZE;

    if(size % BLOCK_SIZE != 0){
        blocks++;
        total = BLOCK_SIZE * blocks;
    }

    size_t b_size = blocks / BLOCK_SIZE;

    if(blocks % BLOCK_SIZE != 0){
        b_size++;
    }

    total += b_size * BLOCK_SIZE;

    if(total > HEAP_SIZE){
        return -1;
    }

    manager.blocks = blocks;
    manager.bitmap = base;
    manager.used = 0;
    manager.start = base + b_size * BLOCK_SIZE * sizeof(uint32_t);
    manager.current =0;

    bitmap_init();

    return 0; 
}


void bitmap_init(){
    for(int i = 0; i < manager.blocks; i++){
        manager.bitmap[i] = FREE;
    }
}

void * mm_malloc(size_t size){

    size_t required = size/BLOCK_SIZE;

    if(size % BLOCK_SIZE !=0){
        required++;
    }

    if(required > manager.blocks - manager.used){
        return NULL;
    }

    uint64_t first = mm_find(required, manager.current, manager.blocks);

    if(first == NULL){
        first = mm_find(required, 0, manager.current);
    }

    if(first == NULL){
        return (void *)NULL;
    }

    manager.used += required;

    return (void *) first;
}


uintptr_t mm_find(size_t required, size_t start, size_t end){

    size_t free = 0;
    size_t first = start;
    size_t i;

    for(i = first; i < end; i++){
        if(manager.bitmap[i] == FREE){
            free++;

            if(free == required){
                mm_fill(required, first);
                manager.current = i;
                return (uintptr_t)(manager.start + first * BLOCK_SIZE);
            }
        }else{
            free= 0;
            first = i + 1;
        }
    }

    manager.current = i;

    if(free == required){
         mm_fill(required, first);
        return (uintptr_t)(manager.start + first * BLOCK_SIZE);
    }

    return NULL;
}


void mm_fill(size_t required, size_t first){
    manager.bitmap[first] = START;
    for(size_t i = first + 1; i < first + required; i++){
        manager.bitmap[i] = ALLOCATED;
    }
}


void mm_free(void * ptr){
    if(ptr == NULL){
         drawWord1("INVALID FREE");
         return;
    }

    size_t index = ((uintptr_t) ptr - (uintptr_t) manager.start) / BLOCK_SIZE;
    
    if(manager.bitmap[index] != START || index >= manager.blocks){
        drawWord1("INVALID FREE");
        return;
    }

    manager.bitmap[index++] = FREE;
    manager.used--;

    while(manager.bitmap[index] == ALLOCATED && index < manager.blocks){
        manager.bitmap[index++] = FREE;
        manager.used--;
    }
    
}

void mm_status(){
    drawWord1("TOTAL MEMORY: ");
    drawNumber(manager.blocks * BLOCK_SIZE);
    newLine();

    drawWord1("TOTAL USED: ");
    drawNumber(manager.used * BLOCK_SIZE);
    newLine();

    drawWord1("TOTAL FREE: ");
    drawNumber(manager.blocks * BLOCK_SIZE - manager.used * BLOCK_SIZE);
    newLine();

    drawWord1("TOTAL TRASH: ");
    drawNumber(trash());
    newLine();
}


int trash(){
    int cant = 0;
    for(int i = 0; i < manager.blocks; i++){
        if(!(manager.bitmap[i] == FREE || manager.bitmap[i] == ALLOCATED || manager.bitmap[i] == START )){
            cant++;
        }
    }
    return cant;
}

#endif