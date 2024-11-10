
#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdint.h>
#include <sys/types.h>
#include "process.h"

#define STACK 4096
#define MAX_PROCESSES 128

enum State
{
    BLOCKED,
    READY,
    RUNNING,
    ZOMBIE,
    KILLED,
    WAITING,
    EXITED,
    BLOCKED_IO
};

typedef struct child_node child_node;

typedef struct PCB
{
    char *name;
    char **argv;
    uint64_t argc;
    pid_t pid;
    pid_t ppid;
    uint64_t rsp;
    uint64_t base;
    uint64_t rip;
    int ground;
    int state;
    int priority;
    int updated;
    int is_waited;
    int *fds;
    struct child_node *child;
} PCB;

typedef struct child_node
{
    PCB *pcb;
    struct child_node *next;
} child_node;

typedef struct process_info {
    pid_t pid;
    char name[32];
    int priority;
    char stack_base[20];
    char rsp[20];
    int state;
    int ground;
} process_info_t;

typedef struct process_list {
    process_info_t* processes;
    size_t count;
} process_list_t;

pid_t create_process(uint64_t fn, int *fds, uint64_t argc, char **argv, int ground);

pid_t kill_process();

void annihilate();

void add_child(PCB *child, PCB *parent);

pid_t kill_process_pid(pid_t pid);

pid_t block_process(pid_t pid);

pid_t unblock_process(pid_t pid);

pid_t wait_pid(pid_t pid_to_wait);

PCB *get_idle();

void hash_map_init();

int add_pcb(pid_t key, PCB *value);

int remove_pcb(pid_t key);

PCB *find_pcb(pid_t key);

void print_processes();

process_list_t* get_process_list();

void free_process_list(process_list_t* list);

void kill_foreground_process();

void block_shell_read();

void wake_up_shell();

#endif