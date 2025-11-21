#ifndef THINKERNEL_H
#define THINKERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Port */
#include "stm32f303x8.h"
#include "core_cm4.h"

/* SysTick */
extern uint32_t systick_ctr;

void init_systick();
void systick_handler();
/* SysTick */

/* Task */
#define MAX_NUM_TASKS ( 32U )   // Range of IDs is 0-31
#define MAX_PRIORITY  ( 255U )  // Range of Priorities is 0-255

typedef enum
{
    TASK_STATE_RUNNING,
    TASK_STATE_READY,
    TASK_STATE_BLOCKED,
    TASK_STATE_TERMINATED
} TaskState_t;

typedef struct Task
{
    uint32_t* stack_ptr;
    uint32_t* stack_limit;
    uint32_t task_id;
    uint32_t priority;
    TaskState_t task_state;
    struct Task* next;
    struct Task* prev;
} Task_t;

extern uint32_t task_bitmap;
extern uint32_t ready_bitmap;

extern Task_t* curr_task_ptr;
extern Task_t* highest_prio_task_ptr;
extern Task_t* ready_list[MAX_NUM_TASKS];
extern Task_t* task_list[MAX_NUM_TASKS];

// TODO: Figure out a way to not expose this function
void init_stack( Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() );

bool create_task( uint32_t task_id, uint32_t priority, Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() );
bool yield_task(uint32_t task_id);
void exit_task();
/* Task */

void context_switch();
void schedule();
void start_thinkernel();

#endif