#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include "include/pipe_manager.h"
#include "memory_manager/include/memory_manager.h"
#include "include/lib.h"
#include "include/fd_manager.h"
#include "Drivers/include/video_driver.h"

named_pipe_t ** global_pipe_table;

void pipe_table_init() {
    global_pipe_table = (named_pipe_t **) mm_malloc(sizeof(named_pipe_t *) * MAX_PIPES);
    for (int i = 0; i < MAX_PIPES; i++) {
        global_pipe_table[i] = NULL;
    }
}

int named_pipe_create(char *name) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (global_pipe_table[i] != NULL && str_cmp(global_pipe_table[i]->name, name) == 0) {
            return -1;
        }
    }

    for (int i = 0; i < MAX_PIPES; i++) {
        if (global_pipe_table[i] == NULL) {
            named_pipe_t *pipe = (named_pipe_t *) mm_malloc(sizeof(named_pipe_t));
            pipe->name = mm_malloc(sizeof(char) * (str_len(name) + 1));
            str_cpy(pipe->name, name);
            pipe->buff = (char *)mm_malloc(BUFFER_SIZE);
            pipe->write_pos = 0;
            pipe->read_pos = 0;
            pipe->write_sem = my_sem_init(1);
            pipe->read_sem = my_sem_init(0);
            pipe->ref_count = 1;
            pipe->fd = fd_allocate(pipe, FD_TYPE_PIPE);
            global_pipe_table[i] = pipe;
            return pipe->fd;
        }
    }
    return -1;
}

int named_pipe_open(char *name) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (global_pipe_table[i] != NULL && (str_cmp(global_pipe_table[i]->name, name) == 0)) {
            global_pipe_table[i]->ref_count++;
            return global_pipe_table[i]->fd;
        }
    }
    return -1;
}

void named_pipe_close(int fd) {
    fd_entry * entry = fd_get_entry(fd);
    if (entry == NULL) {
        return;
    }
    named_pipe_t * pipe = (named_pipe_t *)entry->resource;
    pipe->ref_count--;
    if (pipe->ref_count == 0) {
        // Free resources if no more references to this pipe
        mm_free(pipe->buff);
        my_sem_close(pipe->write_sem);
        my_sem_close(pipe->read_sem);
        mm_free(pipe->name);
        for (int i = 0; i < MAX_PIPES; i++) {
            if (global_pipe_table[i] == pipe) {
                global_pipe_table[i] = NULL;
                break;
            }
        }
        fd_free(pipe->fd);
        global_pipe_table[pipe->index] = NULL;
        mm_free(pipe);
    }
}


ssize_t pipe_read(int fd, char * buff, size_t bytes_r) {
    fd_entry * entry = fd_get_entry(fd);
    if (entry == NULL) {
        return -1;
    }
    named_pipe_t * pipe = entry->resource;

    my_sem_wait(pipe->read_sem);

    size_t bytes_read = 0;
    while (bytes_read < bytes_r && pipe->read_pos != pipe->write_pos) {
        buff[bytes_read++] = pipe->buff[pipe->read_pos];
        pipe->read_pos = (pipe->read_pos + 1) % BUFFER_SIZE;
    }

    my_sem_post(pipe->write_sem);
    return bytes_read;
}

ssize_t pipe_write(int fd, char * buff, size_t bytes_w) {
    fd_entry * entry = fd_get_entry(fd);
    if (entry == NULL) {
        return -1;
    }
    named_pipe_t * pipe = entry->resource;

    my_sem_wait(pipe->write_sem);

    size_t bytes_written = 0;
    while (bytes_written < bytes_w) {
        // Check if buffer would be full after write
        if (((pipe->write_pos + 1) % BUFFER_SIZE) == pipe->read_pos) {
            my_sem_wait(pipe->write_sem);
            continue;
        }
        pipe->buff[pipe->write_pos] = buff[bytes_written++];
        pipe->write_pos = (pipe->write_pos + 1) % BUFFER_SIZE;
    }

    my_sem_post(pipe->read_sem);
    return bytes_written;
}










