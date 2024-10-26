#include <stdint.h>
#include "./include/collections.h"
#include "../memory_manager/include/memory_manager.h"
#include <sys/types.h>
#include "process_manager.h"
#include "../Drivers/include/video_driver.h"


queue_sem_ADT qs_init() {
    queue_sem_ADT queue_ptr =(queue_sem_ADT)mm_malloc(sizeof(queue_sem));
    if (queue_ptr == NULL) {
        return NULL;
    }

    queue_ptr->size = 0;
    queue_ptr->first = NULL;
    queue_ptr->last = NULL;
    return queue_ptr;
}


int sem_insert(pid_t pid, queue_sem_ADT queue) {
    node_sem * new_node = (node_sem *)mm_malloc(sizeof(node_sem));
    if (new_node == NULL) {
        return 0;
    }
    new_node->pid = pid;

    if (queue->first == NULL) {
        new_node->next = new_node;
        queue->first = new_node;
        queue->last = new_node;
    } else {
        new_node->next = queue->first;
        queue->last->next = new_node;
        queue->last = new_node;
    }

    queue->size++;
    return 1;
}


pid_t sem_remove(queue_sem_ADT queue) {
    if (queue == NULL || queue->size == 0) {
        return -1;
    }

    pid_t removed = queue->first->pid;
    node_sem * current = queue->first;
    
    if (queue->size == 1) {
        //mm_free(current);
        queue->first = NULL;
        queue->last = NULL;
    } else {
        queue->first = queue->first->next;
        queue->last->next = queue->first;
        //mm_free(current);
    }
    
    queue->size--;
    return removed;
}

