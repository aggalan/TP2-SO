// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "../include/usr_sys_calls.h"
#include "test_util.h"
#include "../include/lib.h"
#include "./test_sync.h"
#include <stddef.h>

#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory
int sem_id = -1;

void slowInc(int64_t *p, int64_t inc)
{
  uint64_t aux = *p;
  call_nice(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[])
{
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 4)
    return -1;

  if ((n = satoi(argv[1])) <= 0)
    return -1;
  if ((inc = satoi(argv[2])) == 0)
    return -1;
  if ((use_sem = satoi(argv[3])) < 0)
    return -1;

  if (use_sem)
    if (call_sem_open(sem_id) == -1)
    {
      print(0xFFFFFF, "test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++)
  {
    if (use_sem)
    {
      call_sem_wait(sem_id);
    }
    slowInc(&global, inc);
    if (use_sem)
    {
      call_sem_post(sem_id);
    }
  }

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[])
{ //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 4)
    return -1;

  char *argvDec[] = {"dec", argv[1], "-1", argv[2], NULL};
  char *argvInc[] = {"inc", argv[1], "1", argv[2], NULL};

  global = 0;

  if (satoi(argv[2]))
  {
    if ((sem_id = call_sem_init(1)) == -1)
    {
      print(0xFFFFFF, "test_sync: ERROR creating semaphore\n");
      return -1;
    }
  }

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    pids[i] = call_create_process(my_process_inc, 0, 4, argvDec, 0);
    pids[i + TOTAL_PAIR_PROCESSES] = call_create_process(my_process_inc, 0, 4, argvInc, 0);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    call_waitpid(pids[i]);
    call_waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  print(0xFFFFFF, "Final value: %d\n", global);

  if (satoi(argv[2]))
    call_sem_close(sem_id);

  return 1;
}