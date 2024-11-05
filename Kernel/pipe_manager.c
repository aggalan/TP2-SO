#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include "include/pipe_manager.h"
#include "memory_manager/include/memory_manager.h"
#include "include/lib.h"
#include "include/fd_manager.h"
#include "Drivers/include/video_driver.h"
#include "include/scheduler.h"
#include "include/fd_manager.h"
#include "include/scheduler.h"

pipe_t **global_pipe_table;

void pipe_table_init()
{
    global_pipe_table = (pipe_t **)mm_malloc(sizeof(pipe_t *) * MAX_PIPES);
    for (int i = 0; i < MAX_PIPES; i++)
    {
        global_pipe_table[i] = NULL;
    }
}

int named_pipe_create(char *name)
{
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (global_pipe_table[i] != NULL && str_cmp(global_pipe_table[i]->name, name) == 0)
        {
            return -1;
        }
    }

    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (global_pipe_table[i] == NULL)
        {
            pipe_t *pipe = (pipe_t *)mm_malloc(sizeof(pipe_t));
            if (pipe == NULL)
            {
                return -1;
            }
            pipe->name = mm_malloc(sizeof(char) * (str_len(name) + 1));
            if (pipe->name == NULL)
            {
                mm_free(pipe);
                return -1;
            }
            str_cpy(pipe->name, name);
            pipe->buff = (char *)mm_malloc(BUFFER_SIZE * sizeof(char));
            if (pipe->buff == NULL)
            {
                mm_free(pipe->name);
                mm_free(pipe);
                return -1;
            }
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                pipe->buff[i] = 0;
            }
            pipe->write_pos = 0;
            pipe->read_pos = 0;
            pipe->write_sem = my_sem_init(1);
            pipe->read_sem = my_sem_init(0);
            pipe->ref_count = 0;

            pipe->read_pid = -1;
            pipe->write_pid = -1;

            pipe->fd = fd_allocate(pipe, FD_TYPE_PIPE);
            global_pipe_table[i] = pipe;
            return 1;
        }
    }
    return -1;
}

int named_pipe_open(char *name, int mode)
{
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (global_pipe_table[i] != NULL && (str_cmp(global_pipe_table[i]->name, name) == 0))
        {
            pid_t pid = get_current_pid();

            if (mode == READ && global_pipe_table[i]->read_pid == -1 && global_pipe_table[i]->write_pid != pid)
            {
                global_pipe_table[i]->read_pid = pid;
            }
            else if (mode == WRITE && global_pipe_table[i]->write_pid == -1 && global_pipe_table[i]->read_pid != pid)
            {
                global_pipe_table[i]->write_pid = pid;
            }
            else
            {
                return -1;
            }
            global_pipe_table[i]->ref_count++;

            return global_pipe_table[i]->fd;
        }
    }
    return -1;
}

void named_pipe_close(int fd)
{
    fd_entry *entry = fd_get_entry(fd);
    if (entry == NULL)
    {
        return;
    }
    pipe_t *pipe = (pipe_t *)entry->resource;
    pid_t pid = get_current_pid();

    if (pipe->write_pid == pid)
    {
        pipe->write_pid = -1;
//        my_sem_post(pipe->read_sem); //just in case i think necessary
    } else if (pipe->read_pid == pid) {
        pipe->read_pid = -1;
//        my_sem_post(pipe->write_sem); // just in case i think necessary
    } else {
        return;
    }

    pipe->ref_count--;
    if (pipe->ref_count == 0)
    {
        mm_free(pipe->buff);
        my_sem_close(pipe->write_sem);
        my_sem_close(pipe->read_sem);
        mm_free(pipe->name);
        fd_free(pipe->fd);
        global_pipe_table[pipe->index] = NULL;
        mm_free(pipe);
    }
}

ssize_t pipe_read(int fd, char *buff, size_t bytes_r)
{
    fd_entry *entry = fd_get_entry(fd);
    if (entry == NULL)
    {
        return -1;
    }

    pipe_t *pipe = entry->resource;

    pid_t pid = get_current_pid();

    if (pipe->read_pid != pid)
    {
        return -1;
    }

    size_t bytes_read = 0;
    while (bytes_read < bytes_r) {
        my_sem_wait(pipe->read_sem);
//        if (pipe->read_pos != pipe->write_pos) {
//            buff[bytes_read++] = pipe->buff[pipe->read_pos];
//            pipe->read_pos = (pipe->read_pos + 1) % BUFFER_SIZE;
//        }
        buff[bytes_read++] = pipe->buff[pipe->read_pos];
        pipe->read_pos = (pipe->read_pos + 1) % BUFFER_SIZE;
        if (pipe->read_pos == pipe->write_pos) {
            if (pipe->write_pid == -1) {
                return 0;
            }
        }

        my_sem_post(pipe->write_sem);
    }

    return bytes_read;
}

ssize_t pipe_write(int fd, const char *buff, size_t bytes_w)
{
    fd_entry *entry = fd_get_entry(fd);
    if (entry == NULL || bytes_w <= 0)
    {
        return -1;
    }
    pipe_t *pipe = entry->resource;

    pid_t pid = get_current_pid();

    if ((pipe->write_pid != pid || pipe->read_pid == -1))
    {
        return -1;
    }

    size_t bytes_written = 0;
    while (bytes_written < bytes_w) {
        my_sem_wait(pipe->write_sem);
        if ((pipe->write_pos + 1) % BUFFER_SIZE != pipe->read_pos) {
            pipe->buff[pipe->write_pos] = buff[bytes_written++];
            pipe->write_pos = (pipe->write_pos + 1) % BUFFER_SIZE;
        } else if (pipe->read_pid == -1) {
            break;
        }
        my_sem_post(pipe->read_sem);
    }

    return bytes_written;
}


int anon_pipe_create() {
    pipe_t * pipe = (pipe_t *) mm_malloc(sizeof(pipe_t));
    pipe->buff = (char *) mm_malloc(sizeof(char) * BUFFER_SIZE);
    pipe->name = (char *) mm_malloc(sizeof(char) * (str_len("anon") + 1));
    str_cpy(pipe->name, "anon");
    pipe->write_pos = 0;
    pipe->read_pos = 0;
    pipe->write_sem = my_sem_init(1);
    pipe->read_sem = my_sem_init(0);
    pipe->ref_count = 0;
    pipe->index = -1; // not on the table

    pipe->read_pid = -1;
    pipe->write_pid = -1;
    pipe->fd = fd_allocate(pipe, FD_TYPE_ANON_PIPE);
    return pipe->fd;
}

ssize_t anon_pipe_read(int fd, char * buff, size_t len) {
    PCB * pcb = get_current();
    if (pcb->fds[STDIN] != fd) {
        return -1;
    }
    return pipe_read(fd, buff, len);
}

ssize_t anon_pipe_write(int fd, const char * buff, size_t len) {
    PCB * pcb = get_current();
    if (pcb->fds[STDOUT] != fd) {
        return -1;
    }
    return pipe_write(fd, buff, len);
}

void signal_anon_pipe_close(pid_t pid, int fd) {
    fd_entry * entry = fd_get_entry(fd);
    if (entry->fd_type != FD_TYPE_ANON_PIPE) {
        return;
    }
    pipe_t * pipe = entry->resource;
    if (pipe->read_pid == pid) {
        pipe->read_pid = -1;
        my_sem_post(pipe->write_pid);
    } else if (pipe->write_pid == pid) {
        pipe->write_pid = -1;
        my_sem_post(pipe->read_pid);
    } else {
        return;
    }
    pipe->ref_count--;
    if (pipe->ref_count == 0) {
        fd_free(fd);
        mm_free(pipe->buff);
        mm_free(pipe->name);
        mm_free(pipe);
    }
}

void signal_anon_pipe_open(pid_t pid, int fd, int end) {
    fd_entry * entry = fd_get_entry(fd);
    if (entry->fd_type != FD_TYPE_ANON_PIPE) {
        return;
    }
    pipe_t * pipe = entry->resource;
    if (end == STDIN && pipe->read_pid == -1) {
        pipe->read_pid = pid;
    } else if (end == STDOUT && pipe->write_pid == -1) {
        pipe->write_pid = pid;
    } else {
        return;
    }
    pipe->ref_count++;
}
