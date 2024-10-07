#include <stdint.h>
#include "collections.h"
#include "memoryManager.h"


linked_list * ll_init() {
    linked_list * list_ptr =(linked_list *)mm_malloc(sizeof(linked_list));
    if (list_ptr == NULL) {
        //insertar mensaje de error
        return NULL;
    }

    list_ptr->size = 0;
    list_ptr->first = NULL;
    list_ptr->last = NULL;
    list_ptr->current = NULL;
    return list_ptr;
}


void insert(void * data, linked_list * list) {
    node_t * node = (node_t *)mm_malloc(sizeof(node_t));
    if (node == NULL) {
        //msg error
        return NULL;
    }
    node->data = data;

    if (list->first == NULL) {
        node->next = node;
        list->first = node;
        list->last = node;
        list->current = node;
    } else {
        list->last->next = node;
        list->last = node;
        node->next = list->first;
    }
    list->size++;
    return;
}

void remove(void * data, linked_list * list) {
    if (list->first == NULL) {
        return;
    }
    node_t * aux = list->first;

    if (list->size == 1 && aux->data == data) {
        mm_free(list->first);
        list->first = NULL;
        list->last = NULL;
        list->current = NULL;
        list->size = 0;
        return;
    }

    while (aux != list->last) {
        if (aux->next->data == data) {
            if (aux->next == list->current) {
                list->current = list->current->next;
            }
            node_t *to_remove = aux->next;
            aux->next = aux->next->next;
            mm_free(to_remove);
            list->size--;
            return;
        }
        aux = aux->next;
    }
    return;
}

