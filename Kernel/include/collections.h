#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <stdint.h>

typedef struct linked_list * linked_list_ADT;


linked_list_ADT ll_init();

void insert(void * data, linked_list_ADT list);

void remove(void * data, linked_list_ADT list);

#endif