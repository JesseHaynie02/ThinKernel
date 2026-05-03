#include "yield_task_test.h"

#include "main.h"

uint32_t yield1_stack[YIELD_TASK_ONE_STACK_SIZE];
Task_t yield1_tcb;
Task_t* yield1_tcb_ptr = &yield1_tcb;

uint32_t yield2_stack[YIELD_TASK_TWO_STACK_SIZE];
Task_t yield2_tcb;
Task_t* yield2_tcb_ptr = &yield2_tcb;

void yield1_task_test()
{
    while (1)
    {
        blink_led( 1 );
        yield_task( YIELD_TASK_ONE_ID );
    }
}

void yield2_task_test()
{
    while (1)
    {
        blink_led( 2 );
        yield_task( YIELD_TASK_TWO_ID );
    }
}
