#ifndef FD_MANAGER_H
#define FD_MANAGER_H

#define MAX_FD 30
#define FD_TYPE_PIPE 3
#define FD_TYPE_STD 4

#define STDIN 0
#define STDOUT 1
#define ERROUT 2

typedef struct fd_entry{
    int fd_type;
    void * resource;
}fd_entry;

void fd_init();

int fd_allocate(void * resource, int fd_type);

void fd_free(int fd);

fd_entry *fd_get_entry(int fd);

int fd_is_valid(int fd);



#endif