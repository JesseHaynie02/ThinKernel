#include "delay_task_test.h"

#include "main.h"

uint32_t delay3_stack[DELAY_TASK_THREE_STACK_SIZE];
Task_t delay3_tcb;
Task_t* delay3_tcb_ptr = &delay3_tcb;

uint32_t delay4_stack[DELAY_TASK_FOUR_STACK_SIZE];
Task_t delay4_tcb;
Task_t* delay4_tcb_ptr = &delay4_tcb;

void delay3_task_test()
{
    while (1)
    {
        blink_led( 3 );
        delay_task( 10000 );
    }
}

void delay4_task_test()
{
    while (1)
    {
        blink_led( 4 );
        delay_task( 10000 );
    }
}
