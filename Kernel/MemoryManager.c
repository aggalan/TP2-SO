#include "memoryManager.h"
#include "videoDriver.h"
#include <stdio.h>

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
uintptr_t mm_find(size_t required, size_t start);




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

    uintptr_t first = mm_find(required, manager.current);

    if(first == NULL){
        first = mm_find(required, 0);
    }

    if(first == NULL){
        return NULL;
    }

    manager.used += required;

    return (void *) first;
}


uintptr_t mm_find(size_t required, size_t start){

    size_t free = 0;
    size_t first = start;
    size_t i;

    for(i = first; i < manager.blocks; i++){
        if(manager.bitmap[i] == FREE){
            free++;

            if(free == required){
                mm_fill(required, first);
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
         drawWordColor("INVALID FREE", WHITE, RED );
         return;
    }

    size_t index = ((uintptr_t) ptr - (uintptr_t) manager.start) / BLOCK_SIZE;
    
    if(manager.bitmap[index] != START || index >= manager.blocks){
        drawWordColor("INVALID FREE", WHITE, RED );
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
    drawWord("TOTAL MEMORY: ");
    drawNumber(manager.blocks * BLOCK_SIZE);
    newline();

    drawWord("TOTAL USED: ");
    drawNumber(manager.used * BLOCK_SIZE);
    newline();

    drawWord("TOTAL FREE: ");
    drawNumber(manager.blocks * BLOCK_SIZE - manager.used * BLOCK_SIZE);
    newline();

    drawWord("TOTAL TRASH: ");
    drawNumber(trash());
    newline();
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



// typedef struct memory_manager {
//     void * start;
//     uint64_t size;
//     uint64_t bitmap_size;
//     uint32_t qty_blocks;
//     uint32_t used_blocks;
//     uint32_t * bitmap;
//     uint32_t current;
// } memory_manager;

// // typedef enum {
// //     FREE = 0,
// //     ALLOCATED = 1,
// //     START = 2
// // } block_state;

// #define FREE 0
// #define ALLOCATED 1
// #define START 2

// memory_manager mm;


// void allocate_heap() {
//     for (uint32_t i = 0; i < mm.qty_blocks; i++) {
//         mm.bitmap[i] = FREE;
//     }
// }

// void mm_init(void * mem_start, uint64_t mem_size) {

//     uint64_t total_size = mem_size;

//     if (mem_size > HEAP_SIZE) {
//         drawWordColor("NOT ENOUGH MEMORY FOR HEAP INITIALIZATION", WHITE, RED);
//         return;
//     }

//     mm.qty_blocks = mem_size/BLOCK_SIZE;

//     if(mem_size % BLOCK_SIZE != 0) {
//         mm.qty_blocks++;
//         total_size = mm.qty_blocks * BLOCK_SIZE;
//     }

//     mm.bitmap_size = mm.qty_blocks/BLOCK_SIZE;

//     if(mm.qty_blocks % BLOCK_SIZE != 0) {
//         mm.bitmap_size++;
//     }

//     total_size += mm.bitmap_size * BLOCK_SIZE;

//     mm.size = total_size;
//     mm.start = mem_start +  mm.bitmap_size * BLOCK_SIZE * sizeof(uint32_t);
//     mm.used_blocks = 0;
//     mm.bitmap = mem_start;
//     mm.current = 0;

//     allocate_heap();
// }


// void * find_contiguous_mem(uint32_t req_blocks, uint32_t * index) {
//     uint32_t cont_blocks = 0;
//     uint32_t start = mm.current;
//     *index = mm.current;

//     while (cont_blocks < req_blocks) {

//         if(mm.current == 0 && cont_blocks > 0) {
//             cont_blocks = 0;
//         }

//         if (mm.bitmap[mm.current] == FREE) {
//             if (cont_blocks == 0) {
//                 *index = mm.current;
//             }
//             cont_blocks++;
            
//          } else {
//             cont_blocks = 0;
//         }

//         mm.current = (mm.current + 1) % mm.qty_blocks;

//         if (mm.current == start) {
//             if(cont_blocks == req_blocks)
//             {
//                 return (void *)(mm.start + (*index) * BLOCK_SIZE);
//             }
//             return NULL; 
//         }
//     }

//     return (void *)(mm.start + (*index) * BLOCK_SIZE);
// }

// void allocate_mem(uint32_t index, uint32_t blocks) {
//     mm.bitmap[index] = START;
//     for (uint32_t i = 1; i < blocks; i++) {
//         mm.bitmap[index + i] = ALLOCATED;
//     }
// }

// void * mm_malloc(uint32_t size) {
//     uint32_t required_blocks = size/BLOCK_SIZE;
//     uint32_t index = 0;

//     if (required_blocks > mm.qty_blocks - mm.used_blocks) {
//         drawWordColor("NOT ENOUGH MEMORY FOR ALLOCATION", WHITE, RED);
//         return NULL;
//     }

//     if (size % BLOCK_SIZE != 0) {
//         required_blocks++;
//     }

//     void *ptr = find_contiguous_mem(required_blocks, &index);

//     if (ptr == NULL) {  
//         drawWordColor("NOT ENOUGH MEMORY FOR ALLOCATION", WHITE, RED);
//         return NULL;
//     }

//     allocate_mem(index, required_blocks);

//     mm.used_blocks += required_blocks;
        
//     return ptr;

// }


// void mm_free(void * ptr) {
//     uint32_t index = (ptr - mm.start) / BLOCK_SIZE;

//     if (index > mm.qty_blocks || mm.bitmap[index] != START) {
//         drawWordColor("INVALID MEMORY ADDRESS TO FREE", WHITE, RED);
//         return;
//     }

//     do {
//         mm.bitmap[index++] = FREE;
//         mm.used_blocks--;
//     }while (index < mm.qty_blocks && mm.bitmap[index] == ALLOCATED);

// }

// void mm_status() {
//     drawWord("Total Memory: ");
//     drawNumber(mm.qty_blocks*BLOCK_SIZE);
//     newline();
//     drawWord("Total Memory In Use: ");
//     drawNumber(mm.used_blocks*BLOCK_SIZE);
//     newline();
//     drawWord("Total Free: ");
//     drawNumber((mm.qty_blocks - mm.used_blocks)*BLOCK_SIZE);
//     newline();
// //    newline();
// //    while(mm_malloc(4097) != NULL) {
// //        ;
// //    }
// //    drawWord("Total Memory: ");
// //    drawNumber(mm.qty_blocks*BLOCK_SIZE);
// //    newline();
// //    drawWord("Total Memory In Use: ");
// //    drawNumber(mm.used_blocks*BLOCK_SIZE);
// //    newline();
// //    drawWord("Total Free: ");
// //    drawNumber((mm.qty_blocks - mm.used_blocks)*BLOCK_SIZE);
// //    newline();

// //    mm_malloc(20);
// //    drawWord("Total Memory: ");
// //    drawNumber(mm.qty_blocks*BLOCK_SIZE);
// //    newline();
// //    drawWord("Total Memory In Use: ");
// //    drawNumber(mm.used_blocks*BLOCK_SIZE);
// //    newline();
// //    drawWord("Total Free: ");
// //    drawNumber((mm.qty_blocks - mm.used_blocks)*BLOCK_SIZE);
// //    newline();
// //    mm_malloc(40);
// //    drawWord("Total Memory: ");
// //    drawNumber(mm.qty_blocks*BLOCK_SIZE);
// //    newline();
// //    drawWord("Total Memory In Use: ");
// //    drawNumber(mm.used_blocks*BLOCK_SIZE);
// //    newline();
// //    drawWord("Total Free: ");
// //    drawNumber((mm.qty_blocks - mm.used_blocks)*BLOCK_SIZE);
// //    newline();

//      uint64_t total = 0;
//      uint64_t total2 = 0;
// //     uint64_t from = 0;
// //     uint64_t to = 0;

// //     uint64_t to2 = 0;

// //     uint64_t max_continous = 0;
// //     uint64_t start_continous = 0;

// //     int flag = 0;

//      for (uint32_t i = 0; i < mm.qty_blocks; i++) {
// // //        drawWord("bitmap position ");
// // //        drawNumber(i);
// // //        drawWord(" is: ");
// // //        drawNumber(mm.bitmap[i]);
//          if (mm.bitmap[i] != FREE && mm.bitmap[i] != ALLOCATED  && mm.bitmap[i] != START) {
// // //            drawWord(" ACA HAY UN VALOR QUE NO ESTA BIEN: ");
// // //            drawNumber(mm.bitmap[i]);
// // //            drawWord(" en posicion: ");
// // //            drawNumber(i);
// // //            newline();

//              total++;
//          }
//          if (mm.bitmap[i] == FREE ) {
// //             if (total2 == 0) {
// //                 from = i;
// //             }
//              total2++;
// // //            drawWord(" at: ");
// // //            drawNumber(i);
// // //            newline();
// //             to2 = i;
// //             if (max_continous == 0 && !flag) {
// //                 start_continous = i;
// //                 flag = 1;
// // //                drawWord(" CONTINOUS STREAK STARTS AT: ");
// // //                drawNumber(start_continous);
// //             }
// //             max_continous++;
//          }
// //         else if (max_continous != 0){
// // //            drawWord(" having ");
// // //            drawNumber(max_continous);
// // //            drawWord("            ");
// //             max_continous = 0;
// //         }
//      }
//      drawWord("total total : ");
//      drawNumber(total2);
//      newline();
// //     drawWord("from: ");
// //     drawNumber(from);
// //     newline();
// //     drawWord("to: ");
// //     drawNumber(to2);
// //     newline();
//      drawWord("total with garbage : ");
//      drawNumber(total);
//      newline();
//      drawWord("en pos 0 hay!: ");
//      drawNumber(mm.bitmap[0]);
//      newline();
//      drawWord(" mm start: ");
//      ptr_to_string_and_print(mm.start);
//      newline();
//      drawWord(" bitmap start: ");
//      ptr_to_string_and_print(mm.bitmap);
//      newline();
//      drawWord(" bitmap end: ");
//      ptr_to_string_and_print(&mm.bitmap[mm.qty_blocks-1]);
//      newline();


// }
