#include <stdint.h>
#include "./include/collections.h"
#include "../memory_manager/include/memory_manager.h"
#include <sys/types.h>
#include "process_manager.h"
#include "../Drivers/include/video_driver.h"


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


int insert(PCB * data, uint8_t priority, linked_list_ADT list) {


    if (list->first == NULL) {
        node_t * new_node = (node_t *)mm_malloc(sizeof(node_t));
        if (new_node == NULL) {
            return 0;
        }
        new_node->data = data;
        new_node->next = new_node;
        list->first = new_node;
        list->last = new_node;
        list->current = new_node;
        list->size++;
        priority--;
    }

    if (priority == 0) {
        return 1;
    }

    uint8_t interval = list->size/priority;

    if (interval == 0) {
        interval = 1;
    }

    node_t * aux = list->current;


    for (int i = 0; i < priority; i++) {
        for (int j = 0; j < interval; j++) {
            aux = aux->next;
        }
        node_t * new_node = (node_t *)mm_malloc(sizeof(node_t));
        if (new_node == NULL) {
            data->priority = i+1;
            remove(data->pid, list);
            return 0;
        }
        new_node->data = data;

        if (aux == list->last) {
            list->last = new_node;
        }

        new_node->next = aux->next;
        aux->next = new_node;

        aux = new_node;
    }
    list->size+=priority;
//    list->total_size += priority;
    return 1;
}

int remove(pid_t pid_remove, linked_list_ADT list) {

    node_t * node = find(pid_remove, list); //uso este y no el del hashmap porq me sirve mas
    if (node == NULL) {
        return 0;
    }

//    if (list->size == 1) {
//        mm_free(list->first);
//        list->first = NULL;
//        list->last = NULL;
//        list->current = NULL;
//        list->size = 0;
//        return 1;
//    }

    uint8_t priority = node->next->data->priority;
    list->size-= priority;

    do  {
        if (node->next->data->pid == pid_remove) {
            if (node->next == list->current) {
                list->current = list->current->next;
            }

            node_t *to_remove = node->next;
            node->next = node->next->next;
            if (list->first == to_remove) {
                list->first = node->next;
            }
            if (list->last == to_remove) {
                list->last = node; // y ya en la linea anterior nos encargamos de que siga siendo circular la lista
            }
            mm_free(to_remove);
            priority--;
        }
        node = node->next;
    } while (priority > 0);

    if (list->size == 0) {
        list->first = NULL;
        list->last = NULL;
        list->current = NULL;
    }


    return 1;
}

int remove_times(pid_t pid_remove, int times ,linked_list_ADT list) {
    node_t * node = find(pid_remove, list); //uso este y no el del hashmap porq me sirve mas
    if (node == NULL) {
        return 0;
    }

    if (times >= node->next->data->priority) {
        remove(pid_remove, list);
    }

//    if (list->size == 1) {
//        mm_free(list->first);
//        list->first = NULL;
//        list->last = NULL;
//        list->current = NULL;
//        list->size = 0;
//        return 1;
//    }

    list->size -= times;

    do  {
        if (node->next->data->pid == pid_remove) {
            if (node->next == list->current) {
                list->current = list->current->next;
            }

            node_t *to_remove = node->next;
            node->next = node->next->next;
            if (list->first == to_remove) {
                list->first = node->next;
            }
            if (list->last == to_remove) {
                list->last = node; // y ya en la linea anterior nos encargamos de que siga siendo circular la lista
            }
            mm_free(to_remove);
            times--;
        }
        node = node->next;
    } while (times > 0);

    if (list->size == 0) {
        list->first = NULL;
        list->last = NULL;
        list->current = NULL;
    }
    return 1;
}

node_t * find(pid_t pid_find, linked_list_ADT list) {
    if (list->size == 0) {
        return NULL;
    }
    node_t * aux = list->last;
    do {
        if (aux->next->data->pid == pid_find) {
            return aux;
        }
        aux = aux->next;
    }while (aux != list->last);
    return NULL;
}

