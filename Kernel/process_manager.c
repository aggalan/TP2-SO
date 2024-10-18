#include <stdint.h>
#include <sys/types.h>
#include "process_manager.h"
#include "./memory_manager/include/memory_manager.h"
#include "scheduler.h"
#include "lib.h"
#include "interrupts.h"
#include "../Drivers/include/video_driver.h"
#include "./collections/include/collections.h"

hash_map_ADT map;
static pid_t pids = 0;

PCB * idle_proc = NULL;

PCB * create_pcb(void * fn, uint8_t prio, uint64_t argc, char ** argv);
//pid_t create_process(uint64_t fn, int priority, int argc, char **argv);
PCB * get_idle();
void free_PCB(PCB * pcb);


PCB * get_idle(){
    return idle_proc;
}

pid_t create_process(uint64_t fn, int priority, uint64_t argc, char **argv){

    PCB * pcb = (PCB *) mm_malloc(sizeof(PCB));
    if(pcb == NULL){
        return -1;
    }

    pcb->rip = fn;
    pcb->state = READY;
    pcb->priority = priority;
    pcb->pid = pids++;
    pcb->ppid = 0;
    pcb->argv=argv;
    pcb->argc = argc;
    pcb->name = argv[0];

    pcb->base = (uint64_t) mm_malloc(STACK);
    pcb->base += STACK -1;

    pcb->rsp = create_context(pcb->base,pcb->rip, argc, argv);

    if(pcb->pid == 0){
        idle_proc = pcb;
    }else{
        add_pcb(pcb->pid, pcb);
        add_process(pcb, priority);
    }

    return pcb->pid;
}

void annihilate() {
    int max = map->size-1;
    for (int i = 0, j = 0; i < MAX_MAP_SIZE  && j < max; i++) {
        if (map->PCB_arr[i] != NULL) {
            map_node * aux = map->PCB_arr[i];
            while (aux != NULL) {
                if (aux->key != 1) {
                    kill_process_pid(aux->value->pid);
                    j++;
                }
                aux = aux->next;
            }
        }
    }
}


pid_t kill_process() {
    return kill_process_pid(running_process());
}

pid_t kill_process_pid(pid_t pid) {

    PCB * pcb = find_pcb(pid);
    if (pcb == NULL) {
        return -1;
    }
    int state = pcb->state;
    // pcb->state = KILLED;

    remove_process(pid);
    remove_pcb(pid);

    if (state == RUNNING) {
        killed();
        nice();
    }

    return pid;
}

pid_t block_process(pid_t pid){

    PCB * pcb = find_pcb(pid);
    if (pcb == NULL) {
        return -1;
    }

    pcb->state = BLOCKED;
    pid_t process = running_process();
    if(pcb->pid == process){
        nice();
    }
    return pcb->pid;
}

pid_t unblock_process(pid_t pid){

    PCB * pcb = find_pcb(pid);
    if (pcb == NULL || pcb->state != BLOCKED) {
        return -1;
    }

    pcb->state = READY;
    return pcb->pid;
}


void hash_map_init() {
    map = hm_init();
}

void free_PCB(PCB * pcb) { //revisar
    mm_free(pcb->name);
    for (int i = 0; i < pcb->argc-1; i++) {
        mm_free(pcb->argv[i+1]);
    }
    mm_free((void*)(pcb->base - STACK + 1));
    mm_free(pcb->argv);
    mm_free(pcb);
}

int add_pcb(pid_t key, PCB * value) {
    return insert_map(key, value, map);
}

int remove_pcb(pid_t key) {
    PCB * status = remove_map(key, map);
    free_PCB(status);
    return 1; // por ahora ya fue el return status se lo dejo asi aunque ni lo use
}

PCB * find_pcb(pid_t key) {
    return find_map(key, map);
}

void print_processes() {
    drawWord1("There are ");
    drawNumber(map->size);
    drawWord1(" processes in the system");
    newLine();
    drawWord1("PID    ");
    drawWord1("NAME          ");
    drawWord1("PRIORITY   ");
    drawWord1("STACK BASE   ");
    drawWord1("RSP        ");
    drawWord1("IS FOREGROUND");
    newLine();
    for (int i = 0, j = 0; j < map->size && i < MAX_MAP_SIZE; i++) {
        if (map->PCB_arr[i] != NULL) {
            map_node * aux = map->PCB_arr[i];
            while (aux != NULL) {
                drawNumber(aux->key);

                pid_t beaut = aux->key;
                int m = 0;
                while(beaut/10 > 0){
                    beaut = beaut/10;
                    m++;
                }
                for (int z = 0; z < (6-m); z++ ) {
                    drawWord1(" ");
                }
                //name
                drawWord1(aux->value->name);
                for (int z = 0; z < 14-str_len(aux->value->name); z++) {
                    drawWord1(" ");
                }

                //name
                drawNumber(aux->value->priority);
                drawWord1("          ");
                address_to_string((void *)aux->value->base); //legal porque base es uint64_t

                unsigned long beaut2 = aux->value->base;
                m = 0;
                while(beaut2/16 > 0){
                    beaut2 = beaut2/16;
                    m++;
                }
                for (int z = 0; z < (10-m); z++ ) {
                    drawWord1(" ");
                }

                address_to_string((void *)aux->value->rsp); //legal porque rsp es uint64_t

                beaut2 = aux->value->base;
                m = 0;
                while(beaut2/16 > 0){
                    beaut2 = beaut2/16;
                    m++;
                }
                for (int z = 0; z < (8-m); z++ ) {
                    drawWord1(" ");
                }
                drawWord1("FALSE");
                newLine();
                j++;
                aux = aux->next;
            }
        }
    }
}



// #include <stdint.h>
// #include <sys/types.h>
// #include "processManager.h"
// #include "collections.h"
// #include "memoryManager.h"
// #include "scheduler.h"
// #include "lib.h"
// #include "interrupts.h"
// #include "../Drivers/include/videoDriver.h"

// static pid_t pid = 0;

// PCB * create_pcb(void * fn, uint8_t prio, uint64_t argc, char ** argv);
// PCB * create_idle_process();


// pid_t create_process(void * fn, uint8_t prio, uint64_t argc, char ** argv) {

//     PCB * pcb = create_pcb(fn,prio, argc, argv);
//     if (pcb == NULL) {
//         return -1;
//     }

//     add_process(pcb, prio);

//     return pcb->process->pid;
// }

// PCB * create_idle_process(){

//     char ** argv = (char **)mm_malloc(sizeof(char *)*2);
//     if (argv == NULL) {
//         return NULL;
//     }

//     argv[0] = (char *)mm_malloc(5* sizeof(char));
//     if (argv[0] == NULL) {
//         mm_free(argv);
//         return NULL;
//     }

//     str_cpy(argv[0], "idle");

//     return create_pcb(idle, 3, 1, argv);
// }


// PCB * create_pcb(void * fn, uint8_t prio, uint64_t argc, char ** argv) {

//     PCB * pcb = (PCB *)mm_malloc(sizeof(PCB));
//     if (pcb == NULL) {
//         return NULL;
//     }

//     pcb->priority = prio;

//     process * p = (process *)mm_malloc(sizeof(process));
//     if (p == NULL) {
//         mm_free(pcb);
//         return NULL;
//     }

//     p->name = mm_malloc((size_t)str_len(argv[0]) + 1);
//     if (p->name == NULL) {
//         mm_free(p);
//         mm_free(pcb);
//         return NULL;
//     }

//     str_cpy(p->name, argv[0]);

//     p->state = READY;
//     if(pid == 0){//idle
//         p->parent_pid = 1;
//     }else if (pid == 1){ //shell
//         p->parent_pid = 1;
//     }else {
//         p->parent_pid = get_active_pid();
//     }

//     p->pid = pid++;



//     p->heap = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
//     if (p->heap == NULL) {
//         mm_free(p->name);
//         mm_free(p);
//         mm_free(pcb);
//         return NULL;
//     }

//     p->heap->base_ptr = fn;

//     p->heap->current = p->heap->base_ptr;
//     if(p->heap->current == NULL) {
//         mm_free(p->heap);
//         mm_free(p->name);
//         mm_free(p);
//         mm_free(pcb);
//         return NULL;
//     }

//     p->stack = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
//     if (p->stack == NULL) {
//         mm_free(p->heap);
//         mm_free(p->name);
//         mm_free(p);
//         mm_free(pcb);
//         return NULL;
//     }

//     p->stack->base_ptr = mm_malloc(STACK);
//     if (p->stack->base_ptr == NULL) {
//         mm_free(p->stack);
//         mm_free(p->heap);
//         mm_free(p->name);
//         mm_free(p);
//         mm_free(pcb);
//         return NULL;
//     }

//     p->stack->base = p->stack->base_ptr;

//     p->stack->base_ptr += STACK - 1;

//     p->stack->current = create_context(p->stack->base_ptr, fn, argc, argv);

//     pcb->process = p;

//     return pcb;
// }

// pid_t kill_process() {
//     int pid_to_kill = running_process();
//     return kill_process_pid(pid_to_kill);
// }

// pid_t kill_process_pid(pid_t pid_to_free) {

//     PCB * pcb = find_process(pid_to_free);
//     if (pcb == NULL) {
//         return -1;
//     }
//     int state = pcb->process->state;
//     remove_process(pid_to_free, 0); //sino el test de proceso se queda sin memoria porque reserva mas rapido de lo que libera
// //    pcb->process->state = KILLED;
//     if (state == RUNNING) {
//         killed();
//         _irq00Handler();
//     }

//     return pid_to_free;
// }

// pid_t block_process(pid_t pid_to_block){
//     PCB * pcb = find_process(pid_to_block);
//     if (pcb == NULL) {
//         return -1;
//     }
//     if(pcb->process->state == RUNNING){
//         pcb->process->state = BLOCKED;
//          _irq00Handler();
//     }else if(pcb->process->state == READY){
//         pcb->process->state = BLOCKED;
//     }
//     return pcb->process->pid;
// }

// pid_t unblock_process(pid_t pid_to_unblock){
//     PCB * pcb = find_process(pid_to_unblock);
//     if (pcb == NULL) {
//         return -1;
//     }
//     if(pcb->process->state == BLOCKED){
//         pcb->process->state = READY;
//     }
//     return pcb->process->pid;
// }


