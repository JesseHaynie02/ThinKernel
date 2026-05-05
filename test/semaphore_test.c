#include "semaphore_test.h"

#include "main.h"

uint32_t sema5_stack[SEMA_TASK_FIVE_STACK_SIZE];
Task_t sema5_tcb;
Task_t* sema5_tcb_ptr = &sema5_tcb;

uint32_t sema6_stack[SEMA_TASK_SIX_STACK_SIZE];
Task_t sema6_tcb;
Task_t* sema6_tcb_ptr = &sema6_tcb;

Sem_t sema1;
Sem_t* sema1_ptr = &sema1;

Sem_t sema2;
Sem_t* sema2_ptr = &sema2;

void sema5_task_test()
{
    while (1)
    {
        wait_for_semaphore( SEMA_TWO_ID );
        blink_led( 5 );
        post_semaphore( SEMA_ONE_ID );
    }
}

void sema6_task_test()
{
    while (1)
    {
        wait_for_semaphore( SEMA_ONE_ID );
        blink_led( 6 );
        post_semaphore( SEMA_TWO_ID );
    }
}
