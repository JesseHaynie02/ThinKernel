#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define MAX_NUM_TASKS ( 8U )    // Range of IDs is 0-7
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

uint8_t task_list; // 8-bit bit map if bit is 1 task is in use

Task_t* running;
Task_t* ready_list[MAX_NUM_TASKS];
Task_t* blocked_list[MAX_NUM_TASKS];
Task_t* terminated_list[MAX_NUM_TASKS];

bool create_task(uint32_t task_id, uint32_t priority, bool startit, void (*entry_point)());
void run_task(uint32_t task_id);
// void yield_task(uint32_t task_id);

#endif
