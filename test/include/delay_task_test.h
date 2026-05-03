#ifndef DELAY_TASK_TEST_H
#define DELAY_TASK_TEST_H

#include "thinkernel.h"

#define DELAY_TASK_THREE_ID ( 0x3U )
#define DELAY_TASK_FOUR_ID ( 0x4U )

#define DELAY_TASK_THREE_STACK_SIZE ( 0x200U )
#define DELAY_TASK_FOUR_STACK_SIZE ( 0x200U )

extern uint32_t delay3_stack[];
extern Task_t* delay3_tcb_ptr;

extern uint32_t delay4_stack[];
extern Task_t* delay4_tcb_ptr;

void delay3_task_test();
void delay4_task_test();

#endif
