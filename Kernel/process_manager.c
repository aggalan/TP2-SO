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

PCB *idle_proc = NULL;
PCB *shell_process;

PCB *create_pcb(void *fn, uint8_t prio, uint64_t argc, char **argv);
// pid_t create_process(uint64_t fn, int priority, int argc, char **argv);
PCB *get_idle();
void free_PCB(PCB *pcb);
void fetch_milk(PCB *child);
void remove_child(PCB *parent, pid_t pid);

PCB *get_idle()
{
    return idle_proc;
}

pid_t create_process(uint64_t fn, int priority, uint64_t argc, char **argv, int ground)
{

    PCB *pcb = (PCB *)mm_malloc(sizeof(PCB));
    if (pcb == NULL)
    {
       for (int i = 0; i < argc; i++)// he who uncomments this shall bear the sacred burden of uncovering the
       {                             // truth behind the reason as of why this destroys everything.
           mm_free(argv[i]);         // may god bear witness to your brave attempt, for only he, and the person i was when this code was written, know how to fix it
       }                             //godspeed.
       mm_free(argv);
        return -1;
    }

    pcb->rip = fn;
    pcb->state = READY;
    pcb->priority = priority;
    pcb->pid = pids++;
    pcb->argv = argv;
    pcb->argc = argc;
    pcb->name = argv[0];
    pcb->is_waited = 0;
    pcb->child = NULL;
    pcb->ground = ground;
    pcb->updated = 0;

    pcb->base = (uint64_t)mm_malloc(STACK);
    if ((void *)pcb->base == NULL)
    {
        mm_free(pcb);
       for (int i = 0; i < argc; i++)// he who uncomments this shall bear the sacred burden of uncovering the
       {                             // truth behind the reason as of why this destroys everything.
           mm_free(argv[i]);         // may god bear witness to your brave attempt, for only he, and the person i was when this code was written, know how to fix it
       }                             //godspeed.
       mm_free(argv);
        return -1;
    }
    pcb->base += STACK - 1;

    pcb->rsp = create_context(pcb->base, pcb->rip, argc, argv);

    if (pcb->pid != 1)
    {
        PCB *parent = get_current();
        add_child(pcb, parent);
        pcb->ppid = parent->pid;
    } else {
        pcb->ppid = -1;
    }

    if (pcb->pid == 0)
    {
        idle_proc = pcb;
    }
    else
    {
        add_pcb(pcb->pid, pcb);
        add_process(pcb, priority);
    }
    if (pcb->pid == 1)
    {
        shell_process = pcb;
    }

    if (ground && pcb->ppid == 1) {
        wait_pid(pcb->pid);
    }

    return pcb->pid;
}

void add_child(PCB *child, PCB *parent)
{
    child_node *node = (child_node *)mm_malloc(sizeof(child_node));
    node->pcb = child;
    node->next = parent->child;
    parent->child = node;
}

void annihilate()
{
    int max = map->size > 0 ? map->size - 1 : 0;
    for (int i = 0, j = 0; i < MAX_MAP_SIZE && j < max; i++)
    {
        if (map->PCB_arr[i] != NULL)
        {
            map_node *aux = map->PCB_arr[i];
            while (aux != NULL)
            {
                if (aux->key != 1)
                {
                    kill_process_pid(aux->value->pid);
                    j++;
                }
                aux = aux->next;
            }
        }
    }
}

pid_t kill_process()
{
    return kill_process_pid(running_process());
}

pid_t kill_process_pid(pid_t pid)
{
    if (pid <= 1) {
        return 0;
    }

    PCB *pcb = find_pcb(pid);
    if (pcb == NULL)
    {
        return -1;
    }
    int state = pcb->state;
    if (state == ZOMBIE)
    {
        return 0;
    }

    fetch_milk(pcb); // the parent of the children goes out for some milk... he never came back.

    PCB *parent = find_pcb(pcb->ppid);

    if (parent->pid == 1 || pcb->is_waited)
    {
        int is_waited = pcb->is_waited;
        remove_child(parent, pid);
        if (is_waited && parent->state == WAITING) {
            parent->state = READY;
        }
        remove_process(pid);
        remove_pcb(pid);
        if (state == RUNNING)
        {
            nice();
        }
        return pid;
    }


    if (pcb->is_waited)
    {
        remove_child(parent, pid);
        parent->state = READY;
        remove_process(pid);
        remove_pcb(pid);
    } else {
        pcb->state = ZOMBIE;
        remove_process(pid);
    }

    if (state == RUNNING)
    {
        nice();
    }
    return pid;
}

void fetch_milk(PCB *child)
{
    child_node *aux = child->child;
    child_node *to_change = aux;
    while (aux != NULL)
    {
        to_change = aux;
        aux = aux->next;
        if (to_change->pcb->state == ZOMBIE)
        {
            fetch_milk(to_change->pcb);
            remove_pcb(to_change->pcb->pid);
            mm_free(to_change);
        }
        else
        {
            to_change->pcb->ppid = 1;
            to_change->next = shell_process->child;
            shell_process->child = to_change;
        }
    }
}

void remove_child(PCB *parent, pid_t pid)
{
    child_node *aux = parent->child;
    if (aux == NULL) { //porlas, no deberia suceder
        return;
    }
    if (aux->pcb->pid == pid)
    {
        parent->child = aux->next;
        mm_free(aux);
        return;
    }
    while (aux->next != NULL)
    {
        if (aux->next->pcb->pid == pid)
        {
            child_node *to_remove = aux->next;
            aux->next = aux->next->next;
            mm_free(to_remove);
            return;
        }
        aux = aux->next;
    }
}

pid_t block_process(pid_t pid)
{

    if (pid == 1)
    {
        return -1;
    }

    PCB *pcb = find_pcb(pid);
    if (pcb == NULL)
    {
        return -1;
    }

    pcb->updated = 0;
    pcb->state = BLOCKED;
    pid_t process = running_process();
    if (pcb->pid == process)
    {
        nice();
    }
    return pcb->pid;
}

pid_t unblock_process(pid_t pid)
{

    if (pid == 1)
    {
        return -1;
    }

    PCB *pcb = find_pcb(pid);
    if (pcb == NULL || pcb->state != BLOCKED)
    {
        return -1;
    }

    pcb->state = READY;
    return pcb->pid;
}

pid_t wait_pid(pid_t pid_to_wait)
{
    PCB *pcb = find_pcb(pid_to_wait);
    if (pcb == NULL || pcb->ppid != get_current_pid())
    {
        return 0;
    }

    pcb->is_waited = 1;

    if (pcb->state != ZOMBIE)
    {
        PCB *to_wait = get_current();
        to_wait->updated = 0;
        to_wait->state = WAITING;
        nice();
    } else {
        remove_child(find_pcb(pcb->ppid), pcb->pid);
        remove_pcb(pid_to_wait);
    }

    return pcb->pid;
}

void hash_map_init()
{
    map = hm_init();
}

void free_PCB(PCB *pcb)
{ 
//    mm_free(pcb->name);
   for (int i = 0; i < pcb->argc; i++)// he who uncomments this shall bear the sacred burden of uncovering the
   {                                  // truth behind the reason as of why this destroys everything.
       mm_free(pcb->argv[i]);         // may god bear witness to your brave attempt, for only he, and the person i was when this code was written, know how to fix it
   }                                  //godspeed.
   mm_free((void *)(pcb->base - STACK + 1));
   mm_free(pcb->argv);
    mm_free(pcb);
}

int add_pcb(pid_t key, PCB *value)
{
    return insert_map(key, value, map);
}

int remove_pcb(pid_t key)
{
    PCB *status = remove_map(key, map);
    if (status != NULL) { //porlas
        free_PCB(status);
        return 1;
    }
    return 0; // por ahora ya fue el return status se lo dejo asi aunque ni lo use
}

PCB *find_pcb(pid_t key)
{
    return find_map(key, map);
}

void print_processes()
{
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
    for (int i = 0, j = 0; j < map->size && i < MAX_MAP_SIZE; i++)
    {
        if (map->PCB_arr[i] != NULL)
        {
            map_node *aux = map->PCB_arr[i];
            while (aux != NULL)
            {
                drawNumber(aux->key);

                pid_t beaut = aux->key;
                int m = 0;
                while (beaut / 10 > 0)
                {
                    beaut = beaut / 10;
                    m++;
                }
                for (int z = 0; z < (6 - m); z++)
                {
                    drawWord1(" ");
                }
                // name
                drawWord1(aux->value->name);
                for (int z = 0; z < 14 - str_len(aux->value->name); z++)
                {
                    drawWord1(" ");
                }

                // name
                drawNumber(aux->value->priority);
                drawWord1("          ");
                address_to_string((void *)aux->value->base); // legal porque base es uint64_t

                unsigned long beaut2 = aux->value->base;
                m = 0;
                while (beaut2 / 16 > 0)
                {
                    beaut2 = beaut2 / 16;
                    m++;
                }
                for (int z = 0; z < (10 - m); z++)
                {
                    drawWord1(" ");
                }

                address_to_string((void *)aux->value->rsp); // legal porque rsp es uint64_t

                beaut2 = aux->value->base;
                m = 0;
                while (beaut2 / 16 > 0)
                {
                    beaut2 = beaut2 / 16;
                    m++;
                }
                for (int z = 0; z < (8 - m); z++)
                {
                    drawWord1(" ");
                }

                char *buff = NULL;
                switch (aux->value->state)
                {
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
                for (int z = 0; z < 9 - str_len(buff); z++)
                {
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
