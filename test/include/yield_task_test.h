#ifndef YIELD_TASK_TEST_H
#define YIELD_TASK_TEST_H

#include "thinkernel.h"

#define YIELD_TASK_ONE_ID ( 0x1U )
#define YIELD_TASK_TWO_ID ( 0x2U )

#define YIELD_TASK_ONE_STACK_SIZE ( 0x200U )
#define YIELD_TASK_TWO_STACK_SIZE ( 0x200U )

extern uint32_t yield1_stack[];
extern Task_t* yield1_tcb_ptr;

extern uint32_t yield2_stack[];
extern Task_t* yield2_tcb_ptr;

void yield1_task_test();
void yield2_task_test();

#endif
