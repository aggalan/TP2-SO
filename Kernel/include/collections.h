#ifndef COLLECTIONS_H
#define COLLECTIONS_H
#include <stddef.h>

#include <stdint.h>
#include <sys/types.h>
#include "processManager.h"

typedef struct node_t {
    PCB * data;
    struct node_t * next;
}node_t;

typedef struct linked_list{ //es circular que last apunte a first
    node_t * first;
    node_t * last;
    node_t * current;
    uint64_t size;
}linked_list;

typedef struct linked_list * linked_list_ADT;


linked_list_ADT ll_init();

void insert(PCB * data, uint8_t priority,linked_list_ADT list);

void remove(pid_t pid_remove, linked_list_ADT list, int nice);

void modify_priority(pid_t pid, uint8_t new_priority, linked_list_ADT list);

node_t * find(pid_t pid_find, linked_list_ADT list); // returns pointer to previous node

void free_node(node_t * node);

#endif