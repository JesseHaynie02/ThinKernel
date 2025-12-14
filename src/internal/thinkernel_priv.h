#ifndef THINKERNEL_PRIV_H
#define THINKERNEL_PRIV_H

#include "thinkernel.h"

#define BASEPRI_MASK_PENDSV_SYSTICK   14U
#define BASEPRI_UNMASK_ALL            0U

extern uint32_t task_bitmap;
extern uint32_t sem_bitmap;
extern uint32_t ready_bitmap;

extern Task_t* curr_task_ptr;
extern Task_t* highest_prio_task_ptr;
extern Task_t* ready_list[MAX_NUM_TASKS];
extern Task_t* task_list[MAX_NUM_TASKS];
extern Task_t* delay_list;

extern Sem_t* sem_list[MAX_NUM_SEMS];

void init_systick();
void init_stack( Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() );

void context_switch();
void schedule();

#endif
