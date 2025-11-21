#ifndef THINKERNEL_PRIV_H
#define THINKERNEL_PRIV_H

#include "thinkernel.h"

struct Task
{
    uint32_t* stack_ptr;
    uint32_t* stack_limit;
    uint32_t task_id;
    uint32_t priority;
    TaskState_t task_state;
    struct Task* next;
    struct Task* prev;
};

extern uint32_t systick_ctr;

extern uint32_t task_bitmap;
extern uint32_t ready_bitmap;

extern Task_t* curr_task_ptr;
extern Task_t* highest_prio_task_ptr;
extern Task_t* ready_list[MAX_NUM_TASKS];
extern Task_t* task_list[MAX_NUM_TASKS];

void init_systick();
void init_stack( Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() );

void context_switch();
void schedule();

#endif
