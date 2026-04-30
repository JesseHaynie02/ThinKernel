#include "thinkernel.h"
#include "stm32f303x8.h"

#define TASK_ONE_ID ( 0x0U )
#define TASK_TWO_ID ( 0x1U )

#define TASK_ONE_STACK_SIZE ( 0x200U )
#define TASK_TWO_STACK_SIZE ( 0x200U )

#define SEMA_ONE_ID ( 0x0U )
#define SEMA_TWO_ID ( 0x1U )

uint32_t task1_stack[TASK_ONE_STACK_SIZE];
Task_t task1_tcb;
Task_t* task1_tcb_ptr = &task1_tcb;

uint32_t task2_stack[TASK_TWO_STACK_SIZE];
Task_t task2_tcb;
Task_t* task2_tcb_ptr = &task2_tcb;

Sem_t sema1;
Sem_t* sema1_ptr = &sema1;

Sem_t sema2;
Sem_t* sema2_ptr = &sema2;

void task1();
void task2();
