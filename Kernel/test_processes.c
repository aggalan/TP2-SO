#include "videoDriver.h"
#include "syscalls.h"
#include "test_util.h"
#include "processManager.h"
#include "lib.h"
#include "test_processes.h"
#include "memoryManager.h"
#include "scheduler.h"
#include "collections.h"


typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {

    char *argvAux[] = {0};

    // int32_t pid = create_process(idle1, 1, argvAux);

    // kill_process_pid(pid);

    // return 0;



  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  

  if (argc != 1)
    return -1;

  p_rq p_rqs[max_processes];

    while(1){

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      p_rqs[rq].pid = create_process(idle1, 1, argvAux);

      if (p_rqs[rq].pid == -1) {
        mm_status();
        drawWord("test_processes: ERROR creating process\n");
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              if (kill_process_pid(p_rqs[rq].pid) == -1) {
                drawWord("test_processes: ERROR killing process\n");
                return -1;
              }
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              if (block_process(p_rqs[rq].pid) == -1) {
                drawWord("test_processes: ERROR blocking process\n");
                return -1;
              }
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if (unblock_process(p_rqs[rq].pid) == -1) {
            drawWord("test_processes: ERROR unblocking process\n");
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  }
}