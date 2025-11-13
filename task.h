#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define MAX_NUM_TASKS ( 32U )   // Range of IDs is 0-7
#define MAX_PRIORITY  ( 255U )  // Range of Priorities is 0-254

typedef enum
{
    TASK_STATE_RUNNING,
    TASK_STATE_READY,
    TASK_STATE_BLOCKED,
    TASK_STATE_TERMINATED
} TaskState_t;

typedef struct
{
    uint32_t* stack_ptr;
    uint32_t task_id;
    uint32_t priority;
    TaskState_t task_state;
    void (*entry_point)();
} Task_t;

uint32_t task_bitmap;
uint32_t ready_bitmap;

Task_t* curr_task_ptr;
Task_t* ready_list[MAX_NUM_TASKS];

bool create_task(uint32_t task_id, uint32_t priority, bool startit, void (*entry_point)());
void run_task(uint32_t task_id);
// void yield_task(uint32_t task_id);

#endif
