#include <stdint.h>
#include "collections.h"
#include "memoryManager.h"
#include <sys/types.h>
#include "processManager.h"
#include "../Drivers/include/videoDriver.h"


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


void insert(PCB * data, uint8_t priority, linked_list_ADT list) {


    if (list->first == NULL) {
        node_t * new_node = (node_t *)mm_malloc(sizeof(node_t));
        if (new_node == NULL) {
            return;
        }
        new_node->data = data;
        new_node->next = new_node;
        list->first = new_node;
        list->last = new_node;
        list->current = new_node;
        priority--;
    }

    if (priority == 0) {
        return;
    }

    uint8_t interval = list->size/priority;

    if (interval == 0) {
        interval = 1;
    }

    node_t * aux = list->first;

    for (int i = 0; i < priority; i++) {
        for (int j = 0; j < interval; j++) {
            aux = aux->next;
        }
        node_t * new_node = (node_t *)mm_malloc(sizeof(node_t));
        if (new_node == NULL) {
            return; //HAY QUE VER ESTO ALTA PAJA
        }
        new_node->data = data;

        if (aux == list->last) {
            list->last = new_node;
        }

        new_node->next = aux->next;
        aux->next = new_node;

        aux = new_node;

    }

    list->size++;
}

void remove(pid_t pid_remove, linked_list_ADT list) {
    if (list->first == NULL) {
        return;
    }
    PCB * pcb = find(pid_remove, list);
    if (pcb == NULL) {
        return;
    }
    uint8_t priority = pcb->priority;

    node_t * aux = list->last;

    if (list->size == 1 && aux->data->process->pid == pid_remove) {
        free_node(list->first);
        list->first = NULL;
        list->last = NULL;
        list->current = NULL;
        list->size = 0;
        return;
    }

    do  {
        if (aux->next->data->process->pid == pid_remove) {
            if (aux->next == list->current) {
                list->current = list->current->next;
            }

            node_t *to_remove = aux->next;
            aux->next = aux->next->next;
            if (list->first == to_remove) {
                list->first = aux->next;
            }
            if (list->last == to_remove) {
                list->last = aux; // y ya en la linea anterior nos encargamos de que siga siendo circular la lista
            }
            if (priority == 1) {
                free_node(to_remove); // libero todoooo
                list->size--;
                return;
            }
            mm_free(to_remove); //solo libero nodo porq no es el ultimo
            priority--;
        }
        aux = aux->next;
    } while (priority > 0);

}

void remove_nice(pid_t pid_remove_nice, linked_list_ADT list) {
    if (list->first == NULL) {
        return;
    }
    PCB * pcb = find(pid_remove_nice, list);
    if (pcb == NULL) {
        return;
    }
    uint8_t priority = pcb->priority;

    node_t * aux = list->last;

    if (list->size == 1 && aux->data->process->pid == pid_remove_nice) {
        free_node(list->first);
        list->first = NULL;
        list->last = NULL;
        list->current = NULL;
        list->size = 0;
        return;
    }

    do  {
        if (aux->next->data->process->pid == pid_remove_nice) {
            if (aux->next == list->current) {
                list->current = list->current->next;
            }

            node_t *to_remove = aux->next;
            aux->next = aux->next->next;
            if (list->first == to_remove) {
                list->first = aux->next;
            }
            if (list->last == to_remove) {
                list->last = aux; // y ya en la linea anterior nos encargamos de que siga siendo circular la lista
            }
            mm_free(to_remove); //solo libero nodo porq no es el ultimo
            priority--;
        }
        aux = aux->next;
    } while (priority > 0);
    list->size--;
}

PCB * find(pid_t pid_find, linked_list_ADT list) {
    if (list->size == 0) {
        return NULL;
    }
    node_t * aux = list->current;
    do {
        if (aux->data->process->pid == pid_find) {
            return aux->data;
        }
        aux = aux->next;
    }while (aux != list->current);
    return NULL;
}

void free_node(node_t * node) { //revisar
    mm_free(node->data->process->name);
    mm_free(node->data->process->stack->base);
    mm_free(node->data->process->stack);
    mm_free(node->data->process->heap);
    mm_free(node->data->process);
    mm_free(node->data);
    mm_free(node);
}
