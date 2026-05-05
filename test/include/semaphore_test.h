#ifndef SEMAPHORE_TEST_H
#define SEMAPHORE_TEST_H

#include "thinkernel.h"

#define SEMA_TASK_FIVE_ID ( 0x5U )
#define SEMA_TASK_SIX_ID ( 0x6U )

#define SEMA_TASK_FIVE_STACK_SIZE ( 0x100U )
#define SEMA_TASK_SIX_STACK_SIZE ( 0x100U )

#define SEMA_ONE_ID ( 0x1U )
#define SEMA_TWO_ID ( 0x2U )

extern uint32_t sema5_stack[];
extern Task_t* sema5_tcb_ptr;

extern uint32_t sema6_stack[];
extern Task_t* sema6_tcb_ptr;

extern Sem_t* sema1_ptr;
extern Sem_t* sema2_ptr;

void sema5_task_test();
void sema6_task_test();

#endif
