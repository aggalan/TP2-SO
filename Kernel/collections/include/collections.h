#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <stdint.h>
#include <sys/types.h>
#include "process_manager.h"
#include "semaphore.h"

#define MAX_MAP_SIZE 200

typedef struct node_t {
    PCB * data;
    struct node_t * next;
}node_t;

typedef struct linked_list{ //es circular que last apunte a first
    node_t * first;
    node_t * last;
    node_t * current;
    uint64_t size;
//    uint64_t total_size;
}linked_list;

typedef struct linked_list * linked_list_ADT;

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

int insert_map(pid_t key, PCB * value, hash_map_ADT map);

PCB * remove_map(pid_t key, hash_map_ADT map);

PCB * find_map(pid_t key, hash_map_ADT map);

linked_list_ADT ll_init();

int insert(PCB * data, uint8_t priority,linked_list_ADT list);

int remove(pid_t pid_remove, linked_list_ADT list);

int remove_times(pid_t pid_remove, int times ,linked_list_ADT list);

void modify_priority(pid_t pid, uint8_t new_priority, linked_list_ADT list);

node_t * find(pid_t pid_find, linked_list_ADT list); 

void free_node(node_t * node);

typedef struct node_sem {
    pid_t pid;
    struct node_sem * next;
}node_sem;

typedef struct queue_sem{ //es circular que last apunte a first
    node_sem * first;
    node_sem * last;
    uint64_t size;
}queue_sem;

typedef struct queue_sem * queue_sem_ADT;

queue_sem_ADT qs_init();

int sem_insert(pid_t pcb, queue_sem_ADT list);

pid_t sem_remove(queue_sem_ADT list);

#endif  