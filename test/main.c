#include "yield_task_test.h"

void delay(volatile uint32_t count)
{
    while (count--) __asm__("nop");
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
    create_task(YIELD_TASK_ONE_ID, 0x8, yield1_tcb_ptr, yield1_stack, YIELD_TASK_ONE_STACK_SIZE, yield1_task_test);
    create_task(YIELD_TASK_TWO_ID, 0x8, yield2_tcb_ptr, yield2_stack, YIELD_TASK_TWO_STACK_SIZE, yield2_task_test);
#endif

    start_thinkernel();
}
