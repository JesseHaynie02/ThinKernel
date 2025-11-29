#ifndef THINKERNEL_H
#define THINKERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_NUM_TASKS   ( 32U )     // Range of Task IDs is 0-31
#define MAX_PRIORITY    ( 255U )    // Range of Task Priorities is 0-255
#define MAX_NUM_SEMS    ( 32U )     // Range os Semaphore IDs is 0-31

typedef struct Task Task_t;
typedef struct Semaphore Sem_t;

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

struct Semaphore
{
    uint32_t sem_id;
    uint32_t sem_val;
    struct Task* wait_list;
};

extern uint32_t systick_ctr;

bool create_task(uint32_t task_id, uint32_t priority, Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)());
bool yield_task(uint32_t task_id);
bool delay_task(uint32_t ms);
void exit_task();

bool create_semaphore(uint32_t sem_id, Sem_t* semaphore, uint32_t init_val);
bool post_semaphore(uint32_t sem_id);
bool wait_for_semaphore(uint32_t sem_id);

void start_thinkernel();

#endif