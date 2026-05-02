#include "yield_task_test.h"

uint32_t yield1_stack[YIELD_TASK_ONE_STACK_SIZE];
Task_t yield1_tcb;
Task_t* yield1_tcb_ptr = &yield1_tcb;

uint32_t yield2_stack[YIELD_TASK_TWO_STACK_SIZE];
Task_t yield2_tcb;
Task_t* yield2_tcb_ptr = &yield2_tcb;

void yield1_task_test()
{
    uint32_t last_toggle = 0;
    uint32_t yield1_task_ctr = 0;

    while (1)
    {
        uint32_t curr_tick = 0;

        while ( yield1_task_ctr <= 100 )
        {
            curr_tick = systick_ctr;

            if ( (curr_tick - last_toggle) >= 100 )
            {
                last_toggle = curr_tick;

                GPIOB->ODR ^= GPIO_ODR_3;

                yield1_task_ctr++;
            }
        }

        yield1_task_ctr = 0;
        yield_task( YIELD_TASK_ONE_ID );
    }
}

void yield2_task_test()
{
    uint32_t last_toggle = 0;
    uint32_t yield2_task_ctr = 0;

    while (1)
    {
        uint32_t curr_tick = 0;

        while ( yield2_task_ctr <= 50 )
        {
            curr_tick = systick_ctr;

            if ( (curr_tick - last_toggle) >= 200 )
            {
                last_toggle = curr_tick;

                GPIOB->ODR ^= GPIO_ODR_3;

                yield2_task_ctr++;
            }
        }

        yield2_task_ctr = 0;
        yield_task( YIELD_TASK_TWO_ID );
    }
}
