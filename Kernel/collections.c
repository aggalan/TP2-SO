#include <stdint.h>
#include "collections.h"
#include "memoryManager.h"
#include <sys/types.h>
#include "processManager.h"


linked_list_ADT ll_init() {
    linked_list_ADT list_ptr =(linked_list_ADT)mm_malloc(sizeof(linked_list));
    if (list_ptr == NULL) {
        return NULL;
    }

    list_ptr->size = 0;
    list_ptr->first = NULL;
    list_ptr->last = NULL;
    list_ptr->current = NULL;
    return list_ptr;
}


void insert(PCB * data, linked_list_ADT list) {
    node_t * node = (node_t *)mm_malloc(sizeof(node_t));
    if (node == NULL) {
        return;
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

void remove(pid_t pid, linked_list_ADT list) {
    if (list->first == NULL) {
        return;
    }
    node_t * aux = list->first;

    if (list->size == 1 && aux->data->process->pid == pid) { //revisar pero creo que deberiamos buscar por pid
        free_node(list->first);
        list->first = NULL;
        list->last = NULL;
        list->current = NULL;
        list->size = 0;
        return;
    }

    while (aux != list->last) {
        if (aux->next->data->process->pid == pid) {
            if (aux->next == list->current) {
                list->current = list->current->next;
            }
            node_t *to_remove = aux->next;
            aux->next = aux->next->next;
            if (list->last == to_remove) {
                list->last = aux; // y ya en la linea anterior nos encargamos de que siga siendo circular la lista
            }
            free_node(to_remove);
            list->size--;
            return;
        }
        aux = aux->next;
    }
    return;
}

PCB * find(pid_t pid, linked_list_ADT list) {
    if (list->size == 0) {
        return NULL;
    }
    node_t * aux = list->current;
    do {
        if (aux->data->process->pid == pid) {
            return aux->data;
        }
        aux = aux->next;
    }while (aux != list->current);
    return NULL;
}

void free_node(node_t * node) { //revisar
    mm_free(node->data->process->stack->base_ptr);
    mm_free(node->data->process->stack->current);
    mm_free(node->data->process->heap->base_ptr);
    mm_free(node->data->process->heap->current);
    mm_free(node->data->process->stack);
    mm_free(node->data->process->heap);
    mm_free(node->data->process->name);
    mm_free(node->data->process);
    mm_free(node->data);
    mm_free(node);
}
