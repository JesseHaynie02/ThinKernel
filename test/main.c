#include "main.h"

#include "thinkernel.h"
#include "stm32f303x8.h"

#include "yield_task_test.h"
#include "delay_task_test.h"
#include "semaphore_test.h"

void delay( volatile uint32_t count )
{
    while (count--) __asm__("nop");
}

void blink_led( uint32_t blinks_per_cycle )
{
    for (uint32_t i = 0; i < blinks_per_cycle; i++)
    {
        GPIOB->ODR ^= GPIO_ODR_3;
        delay( 250000U );

        GPIOB->ODR ^= GPIO_ODR_3;
        delay( 250000U );
    }

    delay( 2000000U ); // gap between cycles
}

void main()
{
    // Enable GPIO B Port
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Reset GPIO B Port 3 then set as output
    GPIOB->MODER &= ~(GPIO_MODER_MODER3);
    GPIOB->MODER |= GPIO_MODER_MODER3_0;

    // create_semaphore(SEMA_ONE_ID, sema1_ptr, 0);
    // create_semaphore(SEMA_TWO_ID, sema2_ptr, 1);

#ifdef TEST_YIELD_TASK
    create_task(YIELD_TASK_ONE_ID, 8, yield1_tcb_ptr, yield1_stack, YIELD_TASK_ONE_STACK_SIZE, yield1_task_test);
    create_task(YIELD_TASK_TWO_ID, 8, yield2_tcb_ptr, yield2_stack, YIELD_TASK_TWO_STACK_SIZE, yield2_task_test);
#endif

#ifdef TEST_DELAY_TASK
    create_task(DELAY_TASK_THREE_ID, 9, delay3_tcb_ptr, delay3_stack, DELAY_TASK_THREE_STACK_SIZE, delay3_task_test);
    create_task(DELAY_TASK_FOUR_ID, 10, delay4_tcb_ptr, delay4_stack, DELAY_TASK_FOUR_STACK_SIZE, delay4_task_test);
#endif

    start_thinkernel();
}
