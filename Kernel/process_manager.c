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
PCB * shell_process;

PCB * create_pcb(void * fn, uint8_t prio, uint64_t argc, char ** argv);
//pid_t create_process(uint64_t fn, int priority, int argc, char **argv);
PCB * get_idle();
void free_PCB(PCB * pcb);
void fetch_milk(PCB * child);
void remove_child(PCB * parent, pid_t pid);


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
    pcb->argv=argv;
    pcb->argc = argc;
    pcb->name = argv[0];
    pcb->is_waited = 0;

    pcb->base = (uint64_t) mm_malloc(STACK);
    pcb->base += STACK -1;

    pcb->rsp = create_context(pcb->base,pcb->rip, argc, argv);

    if (pcb->pid != 1) {
        PCB * parent = get_current();
        add_child(pcb, parent);
        pcb->ppid = parent->pid;
    }

    if(pcb->pid == 0){
        idle_proc = pcb;
    }else{
        add_pcb(pcb->pid, pcb);
        add_process(pcb, priority);
    }
    if (pcb->pid == 1) {
        shell_process = pcb;
    }

    return pcb->pid;
}

void add_child(PCB * child, PCB * parent) {
    child_node * node = (child_node *) mm_malloc(sizeof(child_node));
    node->pcb = child;
    node->next = parent->child;
    parent->child = node;
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

//    drawWord1(" HERE I AM I SHOULD BE CALLED 4 TIMES ");

    PCB * pcb = find_pcb(pid);
    if (pcb == NULL) {
        return -1;
    }
    int state = pcb->state;
    if (state == ZOMBIE) {
        return 0;
    }
    remove_process(pid);

    PCB * parent = find_pcb(pcb->ppid);


    fetch_milk(pcb); // the parent of the children goes out for some milk... he never came back.


    if (parent->pid == 1) {
        remove_child(parent, pid);
        remove_pcb(pid);
        if (state == RUNNING) {
            killed();
            nice();
        }
        return pid;
    } else {
        pcb->state = ZOMBIE;
        // aca en linux se libera la mayoria de las cosas y te quedas con el return status para que lo levante el padre
        //cabe aclarar que nuestro waitpid solo le pasamos pid creo entonces el parent nunca va a levantar el return status de hijo pero lo disenamos para que si quisiera lo pueda hacer
    }

    if (pcb->is_waited) {
        parent->state = READY;
    }

    if (state == RUNNING) {
        killed();
        nice();
    }


    return pid;
}

void fetch_milk(PCB * child) { //ver esto
    child_node * aux = child->child;
    while (aux != NULL) {
        child_node * to_change = aux;
        aux = aux->next;
        if (to_change->pcb->state == ZOMBIE) {
            remove_pcb(to_change->pcb->pid);
            mm_free(to_change);
        } else {
            to_change->pcb->ppid = 1;
            to_change->next = shell_process->child;
            shell_process->child = to_change;
        }
    }
}

void remove_child(PCB * parent, pid_t pid) {
    child_node * aux = parent->child;
    if (aux->pcb->pid == pid) {
        parent->child = aux->next;
        mm_free(aux);
    }
    while (aux->next != NULL) {
        if (aux->next->pcb->pid == pid) {
            child_node * to_remove = aux->next;
            aux->next = aux->next->next;
            mm_free(to_remove);
            break;
        }
        aux = aux->next;
    }
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

int wait_pid(pid_t pid_to_wait) {
    PCB * pcb = find_pcb(pid_to_wait);
    if (pcb == NULL || pcb->ppid != get_current_pid()) {
        return 0;
    }

    pcb->is_waited = 1;

    PCB * to_wait = get_current();
    to_wait->state = WAITING;

    nice();

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

    drawWord1("STATE    ");

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

                char * buff = NULL;
                switch(aux->value->state) {
                    case READY:
                        buff = "READY";
                        break;
                    case BLOCKED:
                        buff = "BLOCKED";
                        break;
                    case RUNNING:
                        buff = "RUNNING";
                        break;
                    case ZOMBIE:
                        buff = "ZOMBIE";
                        break;
                    case WAITING:
                        buff = "WAITING";
                        break;
                    case EXITED:
                        buff = "EXITED";
                        break;
                }
                drawWord1(buff);
                for (int z = 0; z < 9-str_len(buff); z++) {
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


