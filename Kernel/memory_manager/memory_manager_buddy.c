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
#define PAGE_SIZE (1ULL << MIN_ORDER)

typedef struct Block
{
    uint8_t order;
    struct Block *next;
    block_status status;
} __attribute__((aligned(PAGE_SIZE))) Block; // had a problem with allignement so i searched and got this answer which fixes everything

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
    base_address = (void *)align_up((uint64_t)mem_start, PAGE_SIZE);
    total_memory_size = (mem_size / PAGE_SIZE) * PAGE_SIZE;

    memory_status.total_memory = total_memory_size;
    memory_status.used_memory = 0;
    memory_status.free_memory = total_memory_size;

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
    if (size == 0)
    {
        return NULL;
    }
    uint64_t aligned_size = align_up(size + sizeof(Block), PAGE_SIZE);
    uint64_t order = MIN_ORDER;
    while ((1ULL << order) < aligned_size && order <= MAX_ORDER)
    {
        order++;
    }

    if (order > MAX_ORDER)
    {
        return NULL;
    }

    int index = order - MIN_ORDER;
    while (index < AMOUNT_OF_ORDERS && free_lists[index] == NULL)
    {
        index++;
    }

    if (index >= AMOUNT_OF_ORDERS)
    {
        return NULL;
    }

    Block *block = free_lists[index];
    free_lists[index] = block->next;

    while (index > order - MIN_ORDER)
    {
        index--;
        uint64_t buddy_size = 1ULL << (index + MIN_ORDER);
        Block *buddy = (Block *)align_up((uintptr_t)block + buddy_size, PAGE_SIZE);
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

    return (void *)align_up((uintptr_t)block + sizeof(Block), PAGE_SIZE);
}

void mm_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    ptr = (void *)((uintptr_t)ptr & ~(PAGE_SIZE - 1));
    Block *block = (Block *)((uintptr_t)ptr - sizeof(Block));

    if (block->status != ALLOCATED)
    {
        return;
    }

    uint64_t order = block->order;
    uint64_t block_size = 1ULL << order;

    block->status = FREE;
    memory_status.used_memory -= block_size;
    memory_status.free_memory += block_size;

    while (order < MAX_ORDER)
    {
        uintptr_t buddy_address = ((uintptr_t)block - (uintptr_t)base_address) ^ block_size;
        buddy_address += (uintptr_t)base_address;

        if (buddy_address < (uintptr_t)base_address ||
            buddy_address >= (uintptr_t)base_address + total_memory_size)
        {
            break;
        }

        Block *buddy = (Block *)buddy_address;

        if (buddy->status != FREE || buddy->order != order)
        {
            break;
        }

        Block **prev = &free_lists[order - MIN_ORDER];
        while (*prev && *prev != buddy)
        {
            prev = &(*prev)->next;
        }
        if (!*prev)
        {
            break;
        }
        *prev = buddy->next;

        if (buddy < block)
        {
            block = buddy;
        }

        block->order++;
        block_size <<= 1;
        order++;
    }

    block->next = free_lists[order - MIN_ORDER];
    free_lists[order - MIN_ORDER] = block;
}

memory_status_t * mm_status()
{
    memory_status_t* info = mm_malloc(sizeof(memory_status_t));
    if (info == NULL) return NULL;

    info->total_memory = memory_status.total_memory;
    info->used_memory = memory_status.used_memory;
    info->free_memory = memory_status.free_memory;
    str_cpy(info->allocator_type, "Buddy System");

    return info;
}
#endif