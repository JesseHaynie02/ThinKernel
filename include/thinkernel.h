#ifndef THINKERNEL_H
#define THINKERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_NUM_TASKS ( 32U )   // Range of IDs is 0-31
#define MAX_PRIORITY  ( 255U )  // Range of Priorities is 0-255

/* Task */
typedef struct Task Task_t;

typedef enum
{
    TASK_STATE_RUNNING,
    TASK_STATE_READY,
    TASK_STATE_BLOCKED,
    TASK_STATE_TERMINATED
} TaskState_t;

bool create_task( uint32_t task_id, uint32_t priority, Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() );
bool yield_task(uint32_t task_id);
void exit_task();
/* Task */

void start_thinkernel();

#endif