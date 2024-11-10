#ifndef PIPE_MANAGER_H
#define PIPE_MANAGER_H

#include <stddef.h>
#include <unistd.h>
#include "semaphores.h"

#define MAX_PIPES 20
#define MAX_OPEN_PID 10
#define READ 0
#define WRITE 1
#define BUFFER_SIZE 1024

typedef struct pipe_t
{
    int fd;
    int index;
    char *name;
    char *buff;
    int write_pos;
    int read_pos;
    int write_sem;
    int read_sem;
    pid_t read_pid;
    pid_t write_pid;
    int end_closed;
} pipe_t;

void pipe_table_init();

int named_pipe_create(char *name);

int named_pipe_open(char *name, int mode);

void named_pipe_close(int fd);

ssize_t pipe_read(int fd, char *buff, size_t bytes_r);

ssize_t pipe_write(int fd, const char *buff, size_t bytes_w);

void free_entry(int fd);

int anon_pipe_create();

ssize_t anon_pipe_read(int fd, char *buff, size_t len);

ssize_t anon_pipe_write(int fd, const char *buff, size_t len);

void signal_anon_pipe_open(pid_t pid, int fd, int end);

void signal_anon_pipe_close(pid_t pid, int fd);

#endif