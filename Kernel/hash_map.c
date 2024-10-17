#include "process_manager.h"
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include "hash_map.h"
#include "memory_manager_bitmap.h"
#include "../Drivers/include/video_driver.h"

void free_PCB(PCB * pcb);

size_t hash_func(pid_t key, hash_map_ADT map) {
    return key % MAX_MAP_SIZE;
}

hash_map_ADT hm_init() {
    hash_map_ADT map = (hash_map_ADT)mm_malloc(sizeof(hash_map_ADT));
    if (map == NULL) {
        return NULL;
    }
    map->PCB_arr = (map_node **)mm_malloc(MAX_MAP_SIZE*sizeof(map_node *));
    if (map->PCB_arr == NULL) {
        mm_free(map);
        return NULL;
    }
    map->size = 0;
    for (int i = 0; i < MAX_MAP_SIZE; i++) {
        map->PCB_arr[i] = NULL;
    }
    return map;
}

int insert_map(pid_t key, PCB * value, hash_map_ADT map) {
    size_t hash = hash_func(key, map);

    map_node * new_node = (map_node *)mm_malloc(sizeof(map_node));
    if (new_node == NULL) {
        return 0;
    }
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;
    new_node->prev = NULL;

    map_node * aux = map->PCB_arr[hash];

    if (aux == NULL) {
        map->PCB_arr[hash] = new_node;
    } else {
        new_node->next = aux;
        aux->prev = new_node;
        map->PCB_arr[hash] = new_node;
    }
    map->size++;
    return 1;
}

int remove_map(pid_t key, hash_map_ADT map) {
    size_t hash = hash_func(key, map);
    map_node * aux = map->PCB_arr[hash];
    while (aux != NULL) {
        if (aux->key == key) {
            if (aux == map->PCB_arr[hash]) {
                map->PCB_arr[hash] = aux->next;
            } else {
                aux->prev->next = aux->next;
            }
            if (aux->next != NULL) {
                aux->next->prev = aux->prev;
            }
            free_PCB(aux->value);
            mm_free(aux); //ver si liberamos toda o en la lisa a mi me parece que aca
            map->size--;
            return 1;
        }
        aux = aux->next;
    }
    return 0;
}

PCB * find_map(pid_t key, hash_map_ADT map) {
    size_t hash = hash_func(key, map);
    map_node * aux = map->PCB_arr[hash];
    if (aux == NULL) {
        return NULL;
    }
    while(aux != NULL) {
        if (aux->key == key) {
            return aux->value;
        }
        aux = aux->next;
    }
    return NULL;
}

void free_PCB(PCB * pcb) { //revisar
    mm_free(pcb->name);
    mm_free((void*)(pcb->base - STACK + 1));
    mm_free(pcb->argv);
    mm_free(pcb);
}