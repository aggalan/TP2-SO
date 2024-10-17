#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include "process_manager.h"

#define MAX_MAP_SIZE 200

typedef struct map_node{
    pid_t key;
    PCB * value;
    struct map_node * next;
    struct map_node * prev;
}map_node;

typedef struct hash_map{
    map_node ** PCB_arr;
    size_t size;
}hash_map;

typedef struct hash_map * hash_map_ADT;

hash_map_ADT hm_init();

void insert_map(pid_t key, PCB * value, hash_map_ADT map);

void remove_map(pid_t key, hash_map_ADT map);

PCB * find_map(pid_t key, hash_map_ADT map);


#endif