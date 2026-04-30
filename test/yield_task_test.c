#include "yield_task_test.h"

void task1()
{
    uint32_t last_toggle = 0;
    uint32_t task1_ctr = 0;

    while (1)
    {
        wait_for_semaphore(SEMA_TWO_ID);

        uint32_t curr_tick = 0;

        while (task1_ctr <= 100)
        {
            curr_tick = systick_ctr;

            if ((curr_tick - last_toggle) >= 100)
            {
                last_toggle = curr_tick;

                GPIOB->ODR ^= GPIO_ODR_3;

                task1_ctr++;
            }
        }

        task1_ctr = 0;
        post_semaphore(SEMA_ONE_ID);

        // uint32_t curr_tick = systick_ctr;

        // if ((curr_tick - last_toggle) >= 100)
        // {
        //     last_toggle = curr_tick;

        //     GPIOB->ODR ^= GPIO_ODR_3;

        //     if (++task1_ctr == 100)
        //     {
        //         delay_task(10000);
        //         task1_ctr = 0;
        //     }
        // }
    }
}

void task2()
{
    uint32_t last_toggle = 0;
    uint32_t task2_ctr = 0;

    while (1)
    {
        wait_for_semaphore(SEMA_ONE_ID);

        uint32_t curr_tick = 0;

        while (task2_ctr <= 50)
        {
            curr_tick = systick_ctr;

            if ((curr_tick - last_toggle) >= 200)
            {
                last_toggle = curr_tick;

                GPIOB->ODR ^= GPIO_ODR_3;

                task2_ctr++;
            }
        }

        task2_ctr = 0;
        post_semaphore(SEMA_TWO_ID);

        // uint32_t curr_tick = systick_ctr;

        // if ((curr_tick - last_toggle) >= 200)
        // {
        //     last_toggle = curr_tick;

        //     GPIOB->ODR ^= GPIO_ODR_3;

        //     if (++task2_ctr == 50)
        //     {
        //         delay_task(10000);
        //         task2_ctr = 0;
        //     }
        // }
    }
}
