#include <stdint.h>
#include "collections.h"
#include "memoryManager.h"
#include <sys/types.h>
#include "processManager.h"
#include "videoDriver.h"


ll_ADT ll_init() {

    ll_ADT list =(ll_ADT)mm_malloc(sizeof(ll));
    if (list == NULL) {
        return NULL;
    }
    list->size = 0;
    list->first = NULL;
    list->current = NULL;

    return list;
}


node_t * new_node(void * data){

    node_t * node = (node_t *) mm_malloc(sizeof(node_t));
    if(node == NULL){
        return NULL;
    }
    node->data = data;
    node->next = NULL;
    return node;
}


PCB * next(ll_ADT list){
    if(list->size == 0 || list->current == NULL){
        return NULL;
    }

    // if(list->size == 1)
    //     return list->current;

    PCB * to_return = list->current->data;

    if(list->current->next == NULL){
        list->current = list->first;
    }else{
        list->current = list->current->next;
    }

    return to_return;
}



int insert(PCB * pcb, ll_ADT list){
    node_t * node = new_node(pcb);
    if(node == NULL)
        return 0;

    if(list->first == NULL){
        list->first = node;
        list->current = node; 
    }

    node_t * aux = list->first;

    while(aux->next != NULL){
        aux = aux->next;
    }

    aux->next = node;
    list->size++;

    return 1;
}

int remove(pid_t pid, ll_ADT list) {
    if (list == NULL || list->first == NULL) {
        return 0; 
    }

    node_t * aux = list->first;
    node_t * prev = NULL;

    while (aux != NULL) {
        if (aux->data->pid == pid) {
            list->size--;
            if (prev == NULL) {
                list->first = aux->next;
            } else {
                prev->next = aux->next;
            }
            free_node(aux);
            return 1;  // Nodo eliminado exitosamente
        }
        prev = aux;
        aux = aux->next;
    }

    return 0;  // No se encontró el nodo con el pid dado
}


void free_node(node_t * node){
    free_pcb(node->data->pid);
    mm_free(node);
}


PCB * find(pid_t pid, ll_ADT list) {
    if (list->size == 0) {
        return NULL;
    }

    node_t * aux = list->first;

    while(aux != NULL){
        if(aux->data->pid == pid){
            return aux->data;
        }
        aux = aux->next;
    }

    return NULL;
}











// #include <stdint.h>
// #include "collections.h"
// #include "memoryManager.h"
// #include <sys/types.h>
// #include "processManager.h"


// linked_list_ADT ll_init() {
//     linked_list_ADT list_ptr =(linked_list_ADT)mm_malloc(sizeof(linked_list));
//     if (list_ptr == NULL) {
//         return NULL;
//     }

//     list_ptr->size = 0;
//     list_ptr->first = NULL;
//     list_ptr->last = NULL;
//     list_ptr->current = NULL;
//     return list_ptr;
// }


// void insert(PCB * data, uint8_t priority,linked_list_ADT list) {


//     if (list->first == NULL) {
//         node_t * new_node = (node_t *)mm_malloc(sizeof(node_t));
//         if (new_node == NULL) {
//             return;
//         }
//         new_node->next = new_node;
//         list->first = new_node;
//         list->last = new_node;
//         list->current = new_node;
//         priority--;
//     }

//     if (priority == 0) {
//         return;
//     }

//     uint8_t interval = list->size/priority;

//     if (interval == 0) {
//         interval = 1;
//     }

//     node_t * aux = list->first;

//     for (int i = 0; i < priority; i++) {
//         for (int j = 0; j < interval; j++) {
//             aux = aux->next;
//         }
//         node_t * new_node = (node_t *)mm_malloc(sizeof(node_t));
//         if (new_node == NULL) {
//             return; //HAY QUE VER ESTO ALTA PAJA
//         }
//         new_node->data = data;

//         new_node->next = aux->next;
//         aux->next = new_node;

//         aux = new_node;

//     }
//     list->size++;

// //    if (list->first == NULL) {
// //        node->next = node;
// //        list->first = node;
// //        list->last = node;
// //        list->current = node;
// //    } else {
// //        list->last->next = node;
// //        list->last = node;
// //        node->next = list->first;
// //    }
// //    list->size++;
// //
// //    return;
// }

// void remove(pid_t pid_remove, linked_list_ADT list) {
//     if (list->first == NULL) {
//         return;
//     }
//     PCB * pcb = find(pid_remove, list);
//     if (pcb == NULL) {
//         return;
//     }
//     uint8_t priority = pcb->priority;

//     node_t * aux = list->first;

//     if (list->size == 1 && aux->data->process->pid == pid_remove) {
//         free_node(list->first);
//         list->first = NULL;
//         list->last = NULL;
//         list->current = NULL;
//         list->size = 0;
//         return;
//     }

//     while (aux != list->last) {
//         if (aux->next->data->process->pid == pid_remove) {
//             if (aux->next == list->current) {
//                 list->current = list->current->next;
//             }
//             node_t *to_remove = aux->next;
//             aux->next = aux->next->next;
//             if (list->last == to_remove) {
//                 list->last = aux; // y ya en la linea anterior nos encargamos de que siga siendo circular la lista
//             }
//             if (priority == 1) {
//                 free_node(to_remove); // libero todoooo
//                 return;
//             }
//             mm_free(to_remove); //solo libero nodo porq no es el ultimo
//         }
//         aux = aux->next;
//     }
//     list->size--;
//     return;
// }

// PCB * find(pid_t pid_find, linked_list_ADT list) {
//     if (list->size == 0) {
//         return NULL;
//     }
//     node_t * aux = list->current;
//     do {
//         if (aux->data->process->pid == pid_find) {
//             return aux->data;
//         }
//         aux = aux->next;
//     }while (aux != list->current);
//     return NULL;
// }

// void free_node(node_t * node) { //revisar
//     mm_free(node->data->process->name);
//     mm_free(node->data->process->stack->base);
//     mm_free(node->data->process->stack);
//     mm_free(node->data->process->heap);
//     mm_free(node->data->process);
//     mm_free(node->data);
//     mm_free(node);
// }
