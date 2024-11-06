// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifdef BUDDY
#include "./include/memory_manager.h"
#include "../Drivers/include/video_driver.h"
#include <stdint.h>
#include "lib.h"

#define MAX_ORDER 20
#define MIN_ORDER 5
#define AMOUNT_OF_ORDERS (MAX_ORDER - MIN_ORDER + 1)

typedef struct Block
{
    uint8_t order;
    struct Block *next;
    BlockStatus status;
} Block;

Block *free_lists[AMOUNT_OF_ORDERS];
void *base_address;
uint64_t total_memory_size;
memory_status_t memory_status;

static uint64_t align_up(uint64_t size, uint64_t alignment)
{
    return (size + alignment - 1) & ~(alignment - 1);
}

void mm_init(void *mem_start, uint64_t mem_size)
{
    base_address = mem_start;
    total_memory_size = mem_size;

    memory_status.total_memory = mem_size;
    memory_status.used_memory = 0;
    memory_status.free_memory = mem_size;

    for (int i = 0; i < AMOUNT_OF_ORDERS; i++)
    {
        free_lists[i] = NULL;
    }

    Block *first_block = (Block *)base_address;
    first_block->order = MAX_ORDER;
    first_block->next = NULL;
    first_block->status = FREE;

    free_lists[MAX_ORDER - MIN_ORDER] = first_block;
}

void *mm_malloc(uint32_t size)
{
    if (size == 0 || size > total_memory_size)
    {
        print_kernel(WHITE, "NOT ENOUGH MEMORY FOR ALLOCATION");
        return NULL;
    }

    uint64_t aligned_size = align_up(size, 1ULL << MIN_ORDER);
    uint64_t total_size = aligned_size + sizeof(Block);
    uint64_t order = MIN_ORDER;

    while ((1ULL << order) < total_size && order <= MAX_ORDER)
    {
        order++;
    }

    if (order > MAX_ORDER)
    {
        print_kernel(WHITE, "NOT ENOUGH MEMORY FOR ALLOCATION");
        return NULL;
    }

    int index = order - MIN_ORDER;
    while (index < AMOUNT_OF_ORDERS && free_lists[index] == NULL)
    {
        index++;
    }

    if (index >= AMOUNT_OF_ORDERS)
    {
        print_kernel(WHITE, "NO SUITABLE BLOCK FOUND FOR ALLOCATION");
        return NULL;
    }

    Block *block = free_lists[index];
    free_lists[index] = block->next;

    while (index > order - MIN_ORDER) // splittea un bloque hasta encontrar el tama;o necesario -> necesito dejarlo como free al buddy
    {
        index--;
        uint64_t buddy_offset = 1ULL << (index + MIN_ORDER);
        Block *buddy = (Block *)((uintptr_t)block + buddy_offset);
        buddy->order = index + MIN_ORDER;
        buddy->status = FREE;
        buddy->next = free_lists[index];
        free_lists[index] = buddy;
    }

    block->status = ALLOCATED;
    block->order = order;
    uint64_t block_size = 1ULL << order;
    memory_status.used_memory += block_size;
    memory_status.free_memory -= block_size;

    void *user_ptr = (void *)((uintptr_t)block + sizeof(Block));

    return user_ptr;
}

void mm_free(void *ptr)
{
    if (ptr == NULL || (uintptr_t)ptr < (uintptr_t)base_address ||
        (uintptr_t)ptr >= (uintptr_t)base_address + total_memory_size)
    {
        print_kernel(WHITE, "INVALID MEMORY ADDRESS TO FREE");
        return;
    }

    Block *block = (Block *)((uintptr_t)ptr - sizeof(Block));
    if (block->status != ALLOCATED)
    {
        print_kernel(WHITE, "INVALID MEMORY ADDRESS TO FREE");
        return;
    }

    uint64_t order = block->order;
    uint64_t block_size = 1ULL << order;

    block->status = FREE;
    memory_status.used_memory -= block_size;
    memory_status.free_memory += block_size;

    while (order < MAX_ORDER)
    {
        uintptr_t buddy_address = (uintptr_t)block ^ block_size;

        if (buddy_address < (uintptr_t)base_address || buddy_address >= (uintptr_t)base_address + total_memory_size)
        {
            break;
        }

        Block *buddy = (Block *)buddy_address;

        if (buddy->status != FREE || buddy->order != order)
        {
            break; // Buddy no libre o en otro orden
        }

        Block **prev = &free_lists[order - MIN_ORDER];
        while (*prev && *prev != buddy)
        {
            prev = &(*prev)->next;
        }
        if (*prev)
        {
            *prev = buddy->next;
        }

        if (buddy < block)
        {
            block = buddy; // mergeo ambos
        }

        block->order++;
        block_size <<= 1;
        order++;
    }

    block->next = free_lists[order - MIN_ORDER];
    free_lists[order - MIN_ORDER] = block;
}

void mm_status()
{
    print_kernel(WHITE, "Running on Buddy System\n Memory Status:\n Total Memory: %d\n Used Memory: %d\n Free Memory: %d\n", memory_status.total_memory, memory_status.used_memory, memory_status.free_memory);
}
#endif