#ifndef PIPE_MANAGER_H
#define PIPE_MANAGER_H

#include <stddef.h>
#include <unistd.h>
#include "semaphores.h"

#define MAX_PIPES 20
#define BUFFER_SIZE 4096


typedef struct named_pipe_t {
    int fd;
    int index;
    char * name;
    char * buff;
    int write_pos;
    int read_pos;
    int write_sem;
    int read_sem;
    int ref_count;
}named_pipe_t;

void pipe_table_init();

int named_pipe_create(char *name);

int named_pipe_open(char *name);

void named_pipe_close(int fd);

ssize_t pipe_read(int fd, char * buff, size_t bytes_r);

ssize_t pipe_write(int fd, char * buff, size_t bytes_w);

void free_entry(int fd);

#endif