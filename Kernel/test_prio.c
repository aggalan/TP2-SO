#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include "test_util.h"
#include "processManager.h"
#include "lib.h"
#include "test_processes.h"
#include "videoDriver.h"
#include "scheduler.h"

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 3 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

static void endless_loop_print(uint64_t wait);


void test_prio()
{
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {"test"};
    uint64_t i;

    for (i = 0; i < TOTAL_PROCESSES; i++)
        pids[i] = create_process(endless_loop_print, 1, 1, argv);

    bussy_wait(WAIT);
    newline();
    drawWord("\nCHANGING PRIORITIES...\n");
    newline();

    for (i = 0; i < TOTAL_PROCESSES; i++)
        my_nice(pids[i],3);

    bussy_wait(WAIT);
    newline();
    drawWord("\nBLOCKING...\n");
    newline();

    for (i = 0; i < TOTAL_PROCESSES; i++)
        block_process(pids[i]);

    newline();
    drawWord("CHANGING PRIORITIES WHILE BLOCKED...\n");
    newline();
    for (i = 0; i < TOTAL_PROCESSES; i++)
        my_nice(pids[i], MEDIUM);

    newline();
    drawWord("UNBLOCKING...\n");
    newline();

    for (i = 0; i < TOTAL_PROCESSES; i++)
        unblock_process(pids[i]);

    bussy_wait(WAIT);
    newline();
    drawWord("\nKILLING...\n");
    newline();



    for (i = 0; i < TOTAL_PROCESSES; i++)
        kill_process_pid(pids[i]);

    drawWord("i have killed them all");
    print_processes();
    newline();
}

static void endless_loop_print(uint64_t wait)
{
    while (1)
    {
//        drawWord("PID: ");
//        drawNumber(get_active_pid());
//        newline();
        bussy_wait(wait);
    }
}