#ifndef THINKERNEL_H
#define THINKERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_NUM_TASKS ( 32U )   // Range of IDs is 0-31
#define MAX_PRIORITY  ( 255U )  // Range of Priorities is 0-255

typedef struct Task Task_t;

typedef enum
{
    TASK_STATE_RUNNING,
    TASK_STATE_READY,
    TASK_STATE_BLOCKED,
    TASK_STATE_TERMINATED
} TaskState_t;

struct Task
{
    uint32_t* stack_ptr;
    uint32_t* stack_limit;
    uint32_t task_id;
    uint32_t priority;
    TaskState_t task_state;
    uint32_t delay;
    struct Task* next;
    struct Task* prev;
};

extern uint32_t systick_ctr;

bool create_task( uint32_t task_id, uint32_t priority, Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() );
bool yield_task(uint32_t task_id);
bool delay_task(uint32_t ms);
void exit_task();

void start_thinkernel();

#endif