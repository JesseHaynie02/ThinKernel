#ifndef THINKERNEL_PRIV_H
#define THINKERNEL_PRIV_H

#include "thinkernel.h"

extern uint32_t task_bitmap;
extern uint32_t sem_bitmap;
extern uint32_t ready_bitmap;

extern Task_t* curr_task_ptr;
extern Task_t* highest_prio_task_ptr;
extern Task_t* ready_list[MAX_NUM_TASKS];
extern Task_t* task_list[MAX_NUM_TASKS];

extern Sem_t* sem_list[MAX_NUM_SEMS];

void init_systick();
void init_stack( Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() );
void init_platform();

void enable_ctx_sw();
void disable_ctx_sw();
void context_switch();

bool change_task_state( Task_t* task, TaskState_t change_to_state, Task_t** task_list );

void service_timers();

uint8_t get_highest_bitmap_idx( ready_bitmap );
void schedule();

#endif
