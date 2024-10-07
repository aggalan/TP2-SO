#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <stdint.h>

typedef struct node_t {
    void * data;
    struct node_t * next;
}node_t;

typedef struct linked_list{ //es circular que last apunte a first
    node_t * first;
    node_t * last;
    node_t * current;
    uint64_t size;
}linked_list;

linked_list * ll_init();

void insert(void * data, linked_list * list);

void remove(void * data, linked_list * list);

#endif