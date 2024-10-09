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

void insert(PCB * data, linked_list_ADT list);

void remove(pid_t pid, linked_list_ADT list);

PCB * find(pid_t pid, linked_list_ADT list);

void free_node(node_t * node);

#endif