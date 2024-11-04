// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdint.h>
#include <sys/types.h>
#include "process.h"

#define STACK 4096
#define MAX_PROCESSES 64

enum State
{
    BLOCKED,
    READY,
    RUNNING,
    ZOMBIE,
    KILLED,
    WAITING,
    EXITED
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
    int * fds;
    struct child_node *child;
} PCB;

typedef struct child_node
{
    PCB *pcb;
    struct child_node *next;
} child_node;

pid_t create_process(uint64_t fn, int priority, uint64_t argc, char **argv, int ground);

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

void kill_foreground_process();

void block_shell_read();

void wake_up_shell();

#endif