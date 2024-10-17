#include <stdint.h>
#include <stdio.h>
#include "../include/lib.h"
#include "test_util.h"
#include "../include/usr_sys_calls.h"

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 3 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio()
{
   int64_t pids[TOTAL_PROCESSES];
   char *argv[] = {"test"};
   uint64_t i;

   for (i = 0; i < TOTAL_PROCESSES; i++)
      pids[i] = call_create_process(endless_loop, 2, 1, argv);

   bussy_wait(WAIT);

   print(0xFFFFFF, "\nCHANGING PRIORITIES...\n");

   for (i = 0; i < TOTAL_PROCESSES; i++)
      call_change_priority(pids[i], prio[i]);

   bussy_wait(WAIT);

   print(0xFFFFFF, "\nBLOCKING...\n");

   for (i = 0; i < TOTAL_PROCESSES; i++)
      call_block(pids[i]);

   print(0xFFFFFF, "CHANGING PRIORITIES WHILE BLOCKED...\n");

   for (i = 0; i < TOTAL_PROCESSES; i++)
      call_change_priority(pids[i], MEDIUM);

   print(0xFFFFFF, "UNBLOCKING...\n");

   for (i = 0; i < TOTAL_PROCESSES; i++)
      call_unblock(pids[i]);

   bussy_wait(WAIT);

   print(0xFFFFFF, "\nKILLING...\n");

   for (i = 0; i < TOTAL_PROCESSES; i++)
      call_kill(pids[i]);
}