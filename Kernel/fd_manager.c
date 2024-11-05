// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "include/fd_manager.h"
#include "memory_manager/include/memory_manager.h"
#include "Drivers/include/video_driver.h"

static fd_entry **fd_table;

void fd_init()
{
    fd_table = (fd_entry **)mm_malloc(sizeof(fd_entry *)); // ver de asignar resource aca pero al pedo
    for (int i = 0; i < MAX_FD; i++)
    {
        fd_table[i] = NULL;
    }
    fd_table[0] = (fd_entry *)mm_malloc(sizeof(fd_entry)); // remove magic numbers
    fd_table[0]->fd_type = 0;
    fd_table[1] = (fd_entry *)mm_malloc(sizeof(fd_entry));
    fd_table[1]->fd_type = 1;
    fd_table[2] = (fd_entry *)mm_malloc(sizeof(fd_entry));
    fd_table[2]->fd_type = 2;
}

int fd_allocate(void *resource, int fd_type)
{
    fd_entry *entry = (fd_entry *)mm_malloc(sizeof(fd_entry));
    if (entry == NULL)
    {
        return -1;
    }
    entry->fd_type = fd_type;
    entry->resource = resource;
    for (int i = 3; i < MAX_FD; i++)
    {
        if (fd_table[i] == NULL)
        { // Look for an open slot
            fd_table[i] = entry;
            return i; // Return the fd index
        }
    }
    return -1; // No available file descriptor
}

void fd_free(int fd)
{
    if (fd >= 0 && fd < MAX_FD && fd_table[fd] != NULL)
    {
        mm_free(fd_table[fd]);
        fd_table[fd] = NULL;
    }
}

fd_entry *fd_get_entry(int fd)
{
    if (fd >= 0 && fd < MAX_FD)
    {
        return fd_table[fd];
    }
    return NULL; // Invalid or unassigned file descriptor
}

int fd_is_valid(int fd)
{
    return fd >= 0 && fd < MAX_FD && fd_table[fd] != NULL;
}