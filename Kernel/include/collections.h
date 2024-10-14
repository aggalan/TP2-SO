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

typedef struct ll{ //es circular que last apunte a first
    node_t * first;
    node_t * current;
    size_t size;
}ll;

typedef struct ll * ll_ADT;

ll_ADT ll_init();

node_t * new_node(void * data);

PCB * next(ll_ADT list);

int insert(PCB * pcb, ll_ADT list);

int remove(pid_t pid, ll_ADT list);

void free_node(node_t * node);

PCB * find(pid_t pid, ll_ADT list);




// void insert(PCB * data, uint8_t priority,ll_ADT list);

// void remove(pid_t pid, ll_ADT list);

// void modify_priority(pid_t pid, uint8_t new_priority, ll_ADT list);

// PCB * find(pid_t pid, ll_ADT list);

// void free_node(node_t * node);

#endif








// #ifndef COLLECTIONS_H
// #define COLLECTIONS_H
// #include <stddef.h>

// #include <stdint.h>
// #include <sys/types.h>
// #include "processManager.h"

// typedef struct node_t {
//     PCB * data;
//     struct node_t * next;
// }node_t;

// typedef struct linked_list{ //es circular que last apunte a first
//     node_t * first;
//     node_t * last;
//     node_t * current;
//     uint64_t size;
// }linked_list;

// typedef struct linked_list * linked_list_ADT;


// linked_list_ADT ll_init();

// void insert(PCB * data, uint8_t priority,linked_list_ADT list);

// void remove(pid_t pid, linked_list_ADT list);

// void modify_priority(pid_t pid, uint8_t new_priority, linked_list_ADT list);

// PCB * find(pid_t pid, linked_list_ADT list);

// void free_node(node_t * node);

// #endif